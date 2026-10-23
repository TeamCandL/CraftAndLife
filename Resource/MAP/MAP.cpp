#include "MAP.h"
#include "pch/pch.h"
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <..//Game/Terrain/TerrainMaterial.h>


namespace
{
	constexpr float PI = 3.14159265358979323846f;

	float ClampValue(
	    const float value,
	    const float minValue,
	    const float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	float Lerp(
	    const float a,
	    const float b,
	    const float t)
	{
		return a + (b - a) * t;
	}

	float SmoothStep(
	    float t)
	{
		t = ClampValue(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
}  // namespace

// ============================================================
// MapData
// ============================================================

MapData::MapData()
{
	Generate();
}

int32_t MapData::GetIndex(
    const int32_t x,
    const int32_t z) const
{
	return z * (MapConfig::MAP_WIDTH + 1) + x;
}

bool MapData::IsInsideMap(
    const float x,
    const float z) const
{
	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	return x >= -HALF_WIDTH && x < HALF_WIDTH && z >= -HALF_DEPTH && z < HALF_DEPTH;
}

// ============================================================
// 設計図に基づく地形高度の連続値計算（滑らかな地形）
// ============================================================

float MapData::CalculateRawHeight(
    const float x,
    const float z) const
{
	// 1. 地形全体の緩やかな起伏
	float height = MapConfig::BASE_HEIGHT + std::sin(x * 0.03f) * 1.2f + std::cos(z * 0.035f) * 1.2f;

	// 2. 南西から北東へ向かう傾斜
	const float SLOPE_RATE = ClampValue((x + z + MapConfig::SLOPE_OFFSET) / MapConfig::SLOPE_DISTANCE, 0.0f, 1.0f);

	height += SmoothStep(SLOPE_RATE) * MapConfig::NORTH_EAST_RISE;

	// 3. 左上の渦巻き丘（中心に向かってぐるぐる登るらせん山 ＆ 頂上にボス用の広いアリーナ）
	const float DX = x - MapConfig::SPIRAL_HILL_X;
	const float DZ = z - MapConfig::SPIRAL_HILL_Z;

	const float DIST   = std::sqrt(DX * DX + DZ * DZ);
	const float RADIUS = MapConfig::SPIRAL_HILL_RADIUS;

	if (DIST < RADIUS)
	{
		const float R     = DIST / RADIUS;       // 0.0(中心) ~ 1.0(外周)
		const float ANGLE = std::atan2(DZ, DX);  // -PI ~ PI

		// ====================================================================
		// 【調整エリア①】中心のボス広場（アリーナ）の広さを変えたいとき
		// ====================================================================
		// 0.40f (半径の40%) を大きくすると広場が広くなり、小さくすると狭くなります。
		constexpr float BOSS_ARENA_RATIO = 0.40f;

		// ====================================================================
		// 【調整エリア②】丘全体の高さを変えたいとき
		// ====================================================================
		// 全体の最大高さを変えたい場合は MapConfig::SPIRAL_HILL_HEIGHT の数値を変更するか、
		// 下記の数値を直接書き換えてください。
		const float HILL_MAX_HEIGHT = MapConfig::SPIRAL_HILL_HEIGHT;

		float hillHeight = 0.0f;

		if (R < BOSS_ARENA_RATIO)
		{
			// ================================================================
			// 【調整エリア③】★中心のボス広場の高さを下げたい・変えたいとき★
			// ================================================================
			// 現在は HILL_MAX_HEIGHT がそのまま適用されています。
			// ここから「- 1.5f」のように引いたり、「MapConfig::BASE_HEIGHT + 2.0f」
			// のように直接高さを指定することで、広場だけを低く（または高く）調整できます。
			hillHeight = MapConfig::BASE_HEIGHT + HILL_MAX_HEIGHT;
		}
		else
		{
			// === 外周から中心へ向かってぐるぐる登るらせんスロープ構造 ===
			float outerRatio = (R - BOSS_ARENA_RATIO) / (1.0f - BOSS_ARENA_RATIO);  // 0.0(広場のフチ) ~ 1.0(最外周)

			// 外側（低）から中心（高）に向かって高くなるベース勾配
			float baseSlope = SmoothStep(outerRatio) * HILL_MAX_HEIGHT;

			// ================================================================
			// 【調整エリア④】らせんのスロープのうねり・傾斜を変えたいとき
			// ================================================================
			// ・- outerRatio * 14.0f の「14.0f」を変更すると、らせんの巻き数が変わります。
			// ・- (1.0f - terrace) * 1.5f の「1.5f」を変更すると、スロープの段差の深さが変わります。
			float spiralWave = std::sin(ANGLE * MapConfig::SPIRAL_TURNS - outerRatio * 14.0f);
			float terrace    = std::abs(spiralWave);

			hillHeight = MapConfig::BASE_HEIGHT + baseSlope - (1.0f - terrace) * 1.5f;
		}

		// --------------------------------------------------------
		// 後ろ側の入口（外の平地かららせんスロープの麓へ自然につながる通路）
		// --------------------------------------------------------
		constexpr float ENTRANCE_WIDTH = 11.0f;
		const float ENTRANCE_CENTER_X  = MapConfig::SPIRAL_HILL_X;
		const float ENTRANCE_START_Z   = MapConfig::SPIRAL_HILL_Z + RADIUS + 4.0f;
		const float ENTRANCE_END_Z     = MapConfig::SPIRAL_HILL_Z + RADIUS * 0.3f;

		if (std::abs(x - ENTRANCE_CENTER_X) < ENTRANCE_WIDTH && z <= ENTRANCE_START_Z && z >= ENTRANCE_END_Z)
		{
			float zProgress = (ENTRANCE_START_Z - z) / (ENTRANCE_START_Z - ENTRANCE_END_Z);
			zProgress       = ClampValue(zProgress, 0.0f, 1.0f);

			float entranceHeight =
			    Lerp(MapConfig::BASE_HEIGHT + 1.0f, MapConfig::BASE_HEIGHT + 2.0f, SmoothStep(zProgress));

			float xDistRatio = std::abs(x - ENTRANCE_CENTER_X) / ENTRANCE_WIDTH;
			float xWeight    = SmoothStep(ClampValue(xDistRatio, 0.0f, 1.0f));

			height = Lerp(entranceHeight, height + (hillHeight - MapConfig::BASE_HEIGHT), xWeight);
		}
		else
		{
			height += hillHeight - MapConfig::BASE_HEIGHT;
		}
	}

	// 4. 中央左側の段丘
	const float TERRACE_LEFT_DIST = std::sqrt((x + 12.0f) * (x + 12.0f) + (z - 5.0f) * (z - 5.0f));

	if (TERRACE_LEFT_DIST < 24.0f)
	{
		const float R = TERRACE_LEFT_DIST / 24.0f;
		height += (1.0f - SmoothStep(R)) * 3.0f;
	}

	// 5. 中央右側の段丘
	const float TERRACE_RIGHT_DIST = std::sqrt((x - 35.0f) * (x - 35.0f) + (z - 22.0f) * (z - 22.0f));

	if (TERRACE_RIGHT_DIST < 25.0f)
	{
		height += (1.0f - SmoothStep(TERRACE_RIGHT_DIST / 25.0f)) * 4.0f;
	}

	// 6. 北東側の山
	const float MOUNTAIN_X      = 58.0f;
	const float MOUNTAIN_Z      = 65.0f;
	const float MOUNTAIN_RADIUS = 38.0f;

	const float MX = x - MOUNTAIN_X;
	const float MZ = z - MOUNTAIN_Z;
	const float MD = std::sqrt(MX * MX + MZ * MZ);

	if (MD < MOUNTAIN_RADIUS)
	{
		const float R = MD / MOUNTAIN_RADIUS;

		// 段々の山を作る
		const float RAW_MOUNTAIN = (1.0f - SmoothStep(R)) * 8.0f;

		const float TERRACED_MOUNTAIN = std::floor(RAW_MOUNTAIN / MapConfig::VOXEL_STEP) * MapConfig::VOXEL_STEP;

		height += TERRACED_MOUNTAIN;
	}

	// ============================================================
	// 川の地形
	// 深い溝や垂直な崖を作らず、緩やかに低くする
	// ============================================================

	const float RIVER_CENTER =
	    MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A +
	    std::sin(z * MapConfig::RIVER_FREQUENCY_B + MapConfig::RIVER_PHASE_B) * MapConfig::RIVER_BEND_B;

	const float RIVER_DIST = std::abs(x - RIVER_CENTER);

	// 川岸を広く取り、緩やかに低くする
	constexpr float RIVER_BANK_WIDTH = 14.0f;
	constexpr float RIVER_DEPTH      = 0.8f;

	const float RIVER_RATE = ClampValue(RIVER_DIST / RIVER_BANK_WIDTH, 0.0f, 1.0f);

	height -= (1.0f - SmoothStep(RIVER_RATE)) * RIVER_DEPTH;

	return ClampValue(
	    std::floor(height / MapConfig::VOXEL_STEP) * MapConfig::VOXEL_STEP,
	    MapConfig::BASE_HEIGHT,
	    MapConfig::MAX_HEIGHT);
}

// ============================================================
// 地形生成（滑らかな連続値化処理）
// ============================================================

void MapData::Generate()
{
	// ========================================================
	// 1. 頂点数を計算
	// ========================================================

	const int32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;

	const int32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	// ========================================================
	// 2. 地形の高さを生成
	// 高さを整数に切り捨てず、自然な起伏を維持する
	// ========================================================

	heights.resize(static_cast<size_t>(VERTEX_WIDTH) * static_cast<size_t>(VERTEX_DEPTH));

	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			// 川・丘・山を含めた高さを計算
			const float HEIGHT = CalculateRawHeight(WORLD_X, WORLD_Z);

			// floor() による切り捨ては行わない
			heights[GetIndex(x, z)] = HEIGHT;
		}
	}

	// ========================================================
	// 3. 道の高さを設定
	// 道も周囲の地形に沿わせる
	// ========================================================

	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			if (IsPath(WORLD_X, WORLD_Z))
			{
				heights[GetIndex(x, z)] = CalculateRawHeight(WORLD_X, WORLD_Z);
			}
		}
	}

	// 滝専用の高さ変更処理は行わない
	// 川の谷は CalculateRawHeight() で計算する
}

// ============================================================
// 高さの補間取得
// ============================================================

float MapData::GetHeight(
    const float x,
    const float z) const
{
	if (!IsInsideMap(x, z) || heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	const int32_t GRID_X =
	    ClampValue(static_cast<int32_t>((x + HALF_WIDTH) / MapConfig::TILE_SIZE), 0, MapConfig::MAP_WIDTH);
	const int32_t GRID_Z =
	    ClampValue(static_cast<int32_t>((z + HALF_DEPTH) / MapConfig::TILE_SIZE), 0, MapConfig::MAP_DEPTH);

	return heights[GetIndex(GRID_X, GRID_Z)];
}

// ============================================================
// 道・川の判定[cite: 1]
// ============================================================

bool MapData::IsPath(
    const float x,
    const float z) const
{
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;
	const float T          = ClampValue((z + HALF_DEPTH) / (MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE), 0.0f, 1.0f);

	const float PATH_CENTER_X = Lerp(MapConfig::PATH_START_X, MapConfig::PATH_GOAL_X, T) +
	                            std::sin(T * 3.0f * PI) * 18.0f + std::cos(T * 1.5f * PI) * 10.0f;

	return std::abs(x - PATH_CENTER_X) <= MapConfig::PATH_WIDTH * 0.5f;
}

bool MapData::IsRiver(
    const float x,
    const float z) const
{
	const float RIVER_CENTER =
	    MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A +
	    std::sin(z * MapConfig::RIVER_FREQUENCY_B + MapConfig::RIVER_PHASE_B) * MapConfig::RIVER_BEND_B;

	return std::abs(x - RIVER_CENTER) <= MapConfig::RIVER_WIDTH * 0.5f;
}

// ============================================================
// スタート・ゴール位置[cite: 1]
// ============================================================

Vec3 MapData::GetStartPosition() const
{
	const float X = MapConfig::PATH_START_X;
	const float Z = MapConfig::PATH_START_Z;
	return Vec3{ X, GetHeight(X, Z), Z };
}

Vec3 MapData::GetGoalPosition() const
{
	const float X = MapConfig::PATH_GOAL_X;
	const float Z = MapConfig::PATH_GOAL_Z;
	return Vec3{ X, GetHeight(X, Z), Z };
}

// ============================================================
// MAP メッシュ構築・描画処理
// ============================================================

void MAP::Start()
{
	MaterialResource material{};
	std::unique_ptr<TerrainMaterialProperty> pModelProperty = std::make_unique<TerrainMaterialProperty>();
	pModelProperty->Get().hasTexture                        = { TRUE, TRUE, TRUE, TRUE };
	pModelProperty->Get().baseColor                         = { 1, 1, 1, 1 };
	material.TryLoad(std::move(pModelProperty));
	this->material = Resource<Material>{ std::move(material) };

	MaterialResource waterMaterialResource{};
	std::unique_ptr<TerrainMaterialProperty> pWaterProperty = std::make_unique<TerrainMaterialProperty>();
	pWaterProperty->Get().hasTexture                        = { FALSE, FALSE, FALSE, FALSE };
	pWaterProperty->Get().baseColor                         = { 0.1f, 0.5f, 0.95f, 0.85f };
	waterMaterialResource.TryLoad(std::move(pWaterProperty));
	this->waterMaterial = Resource<Material>{ std::move(waterMaterialResource) };

	Reload();
}

void MAP::Update() {}

void MAP::Draw()
{
	Dx11Manager& DX11 = Get<Dx11Manager>();

	DX11.SetMaterial(transform.GetMaterial());
	DX11.SetMaterial(material);
	DX11.SetShader(shader);
	DX11.SetSurface(surface);
	DX11.SetTexture(texture);
	DX11.Draw();

	DX11.SetMaterial(waterMaterial);
	DX11.SetShader(shader);
	DX11.SetSurface(waterSurface);
	DX11.Draw();
}

void MAP::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& _pDevice)
	    {
		    using Vertex = ModelVertex;

		    const uint32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;
		    const uint32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;
		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

		    std::vector<Vertex> vertices(VERTEX_COUNT);
		    const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;
		    const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

		    for (uint32_t z = 0; z < VERTEX_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH; ++x)
			    {
				    const uint32_t INDEX = x + z * VERTEX_WIDTH;

				    Vec3 pos{};
				    pos.x = x * MapConfig::TILE_SIZE - HALF_WIDTH;
				    pos.z = z * MapConfig::TILE_SIZE - HALF_DEPTH;
				    pos.y = heights[GetIndex(static_cast<int32_t>(x), static_cast<int32_t>(z))];

				    vertices[INDEX].position = pos;
				    vertices[INDEX].uv       = Vec2{ x * MapConfig::TILE_SIZE, z * MapConfig::TILE_SIZE };
				    vertices[INDEX].normal   = Vec3::Up();
			    }
		    }

		    const uint32_t INDEX_COUNT = MapConfig::MAP_WIDTH * MapConfig::MAP_DEPTH * 6;
		    std::vector<uint32_t> indices(INDEX_COUNT);
		    uint32_t idx = 0;

		    for (uint32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
			    {
				    const uint32_t TOP_LEFT     = z * VERTEX_WIDTH + x;
				    const uint32_t TOP_RIGHT    = TOP_LEFT + 1;
				    const uint32_t BOTTOM_LEFT  = (z + 1) * VERTEX_WIDTH + x;
				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    indices[idx++] = TOP_LEFT;
				    indices[idx++] = TOP_RIGHT;
				    indices[idx++] = BOTTOM_LEFT;

				    indices[idx++] = TOP_RIGHT;
				    indices[idx++] = BOTTOM_RIGHT;
				    indices[idx++] = BOTTOM_LEFT;
			    }
		    }

		    // 滑らかな連続地形用の通常法線計算（面法線の平均化）
		    for (size_t i = 0; i < indices.size(); i += 3)
		    {
			    const uint32_t I0 = indices[i];
			    const uint32_t I1 = indices[i + 1];
			    const uint32_t I2 = indices[i + 2];

			    const Vec3 V0 = vertices[I0].position.v;
			    const Vec3 V1 = vertices[I1].position.v;
			    const Vec3 V2 = vertices[I2].position.v;

			    const Vec3 EDGE1       = V1 - V0;
			    const Vec3 EDGE2       = V2 - V0;
			    const Vec3 FACE_NORMAL = DirectX::XMVector3Cross(EDGE1, EDGE2);

			    vertices[I0].normal = Vec3{ vertices[I0].normal.v } + FACE_NORMAL;
			    vertices[I1].normal = Vec3{ vertices[I1].normal.v } + FACE_NORMAL;
			    vertices[I2].normal = Vec3{ vertices[I2].normal.v } + FACE_NORMAL;
		    }

		    for (Vertex& v : vertices)
		    {
			    DirectX::XMVECTOR normalVec =
			        DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(v.normal.v));
			    v.normal = Vec3{ DirectX::XMVector3Normalize(normalVec) };
		    }

		    // D3D11 バッファ作成 (地形)
		    const D3D11_BUFFER_DESC VB_DESC{
			    .ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			    .Usage     = D3D11_USAGE_DEFAULT,
			    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
		    };
		    ComPtr<ID3D11Buffer> pVertexBuffer{};
		    const D3D11_SUBRESOURCE_DATA VB_DATA{ .pSysMem = vertices.data() };
		    _pDevice->CreateBuffer(&VB_DESC, &VB_DATA, pVertexBuffer.GetAddressOf());

		    const D3D11_BUFFER_DESC IB_DESC{
			    .ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			    .Usage     = D3D11_USAGE_DEFAULT,
			    .BindFlags = D3D11_BIND_INDEX_BUFFER,
		    };
		    ComPtr<ID3D11Buffer> pIndexBuffer{};
		    const D3D11_SUBRESOURCE_DATA IB_DATA{ .pSysMem = indices.data() };
		    _pDevice->CreateBuffer(&IB_DESC, &IB_DATA, pIndexBuffer.GetAddressOf());

		    this->surface = Resource<Surface>{ SurfaceResource{
			    .pVertexBuffer = pVertexBuffer,
			    .pIndexBuffer  = pIndexBuffer,
			    .size          = static_cast<UINT>(sizeof(Vertex)),
			    .offset        = 0,
			    .vertexCount   = static_cast<UINT>(vertices.size()),
			    .indexCount    = static_cast<UINT>(indices.size()),
			    .toplogy       = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			} };

		    // 川・滝のメッシュ生成[cite: 1]
		    std::vector<Vertex> waterVertices{};
		    std::vector<uint32_t> waterIndices{};

		    const float HALF_RIVER_W  = MapConfig::RIVER_WIDTH * 0.5f;
		    const float WATER_START_Z = MapConfig::MAP_DEPTH * 0.5f;
		    const float WATER_END_Z   = -MapConfig::MAP_DEPTH * 0.5f;

		    auto GetRiverCenterX = [](const float z)
		    {
			    return MapConfig::RIVER_CENTER_X +
			           std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A +
			           std::sin(z * MapConfig::RIVER_FREQUENCY_B + MapConfig::RIVER_PHASE_B) * MapConfig::RIVER_BEND_B;
		    };

		    auto AddWaterQuad =
		        [&waterVertices, &waterIndices](const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d)
		    {
			    const uint32_t BASE = static_cast<uint32_t>(waterVertices.size());
			    const Vec3 pos[4]   = { a, b, c, d };
			    for (int32_t i = 0; i < 4; ++i)
			    {
				    Vertex v{};
				    v.position = pos[i];
				    v.uv       = Vec2{ pos[i].x, pos[i].z };
				    v.normal   = Vec3::Up();
				    waterVertices.push_back(v);
			    }
			    waterIndices.push_back(BASE + 0);
			    waterIndices.push_back(BASE + 1);
			    waterIndices.push_back(BASE + 2);
			    waterIndices.push_back(BASE + 1);
			    waterIndices.push_back(BASE + 3);
			    waterIndices.push_back(BASE + 2);
		    };

		    for (float z = WATER_START_Z; z > WATER_END_Z; z -= MapConfig::TILE_SIZE)
		    {
			    const float NEXT_Z = std::max(WATER_END_Z, z - MapConfig::TILE_SIZE);
			    const float CX_A   = GetRiverCenterX(z);
			    const float CX_B   = GetRiverCenterX(NEXT_Z);

			    const float LX_A = CX_A - HALF_RIVER_W, RX_A = CX_A + HALF_RIVER_W;
			    const float LX_B = CX_B - HALF_RIVER_W, RX_B = CX_B + HALF_RIVER_W;

			    const float Y_A = GetHeight(CX_A, z) + MapConfig::WATER_SURFACE_OFFSET;
			    const float Y_B = GetHeight(CX_B, NEXT_Z) + MapConfig::WATER_SURFACE_OFFSET;

			    AddWaterQuad(
			        Vec3{ LX_A, Y_A, z },
			        Vec3{ RX_A, Y_A, z },
			        Vec3{ LX_B, Y_B, NEXT_Z },
			        Vec3{ RX_B, Y_B, NEXT_Z });
		    }

		    // 滝の水面（垂直方向）[cite: 1]
		    const float TIER_LEN =
		        (MapConfig::WATERFALL_TOP_Z - MapConfig::WATERFALL_BOTTOM_Z) / MapConfig::WATERFALL_TIERS;
		    for (int32_t tier = 1; tier < MapConfig::WATERFALL_TIERS; ++tier)
		    {
			    const float Z  = MapConfig::WATERFALL_TOP_Z - TIER_LEN * tier;
			    const float CX = GetRiverCenterX(Z);
			    const float LX = CX - HALF_RIVER_W, RX = CX + HALF_RIVER_W;

			    const float UPPER_Y = GetHeight(CX, Z + MapConfig::TILE_SIZE) + MapConfig::WATER_SURFACE_OFFSET;
			    const float LOWER_Y = GetHeight(CX, Z - MapConfig::TILE_SIZE) + MapConfig::WATER_SURFACE_OFFSET;

			    if (UPPER_Y > LOWER_Y + 0.1f)
			    {
				    AddWaterQuad(
				        Vec3{ LX, UPPER_Y, Z },
				        Vec3{ RX, UPPER_Y, Z },
				        Vec3{ LX, LOWER_Y, Z },
				        Vec3{ RX, LOWER_Y, Z });
			    }
		    }

		    // D3D11 バッファ作成 (水面)
		    ComPtr<ID3D11Buffer> pWaterVB{}, pWaterIB{};
		    const D3D11_SUBRESOURCE_DATA W_VB_DATA{ .pSysMem = waterVertices.data() };
		    const D3D11_SUBRESOURCE_DATA W_IB_DATA{ .pSysMem = waterIndices.data() };

		    const D3D11_BUFFER_DESC W_VB_DESC{
			    .ByteWidth = static_cast<UINT>(waterVertices.size() * sizeof(Vertex)),
			    .Usage     = D3D11_USAGE_DEFAULT,
			    .BindFlags = D3D11_BIND_VERTEX_BUFFER,
		    };
		    _pDevice->CreateBuffer(&W_VB_DESC, &W_VB_DATA, pWaterVB.GetAddressOf());

		    const D3D11_BUFFER_DESC W_IB_DESC{
			    .ByteWidth = static_cast<UINT>(waterIndices.size() * sizeof(uint32_t)),
			    .Usage     = D3D11_USAGE_DEFAULT,
			    .BindFlags = D3D11_BIND_INDEX_BUFFER,
		    };
		    _pDevice->CreateBuffer(&W_IB_DESC, &W_IB_DATA, pWaterIB.GetAddressOf());

		    this->waterSurface = Resource<Surface>{ SurfaceResource{
			    .pVertexBuffer = pWaterVB,
			    .pIndexBuffer  = pWaterIB,
			    .size          = static_cast<UINT>(sizeof(Vertex)),
			    .offset        = 0,
			    .vertexCount   = static_cast<UINT>(waterVertices.size()),
			    .indexCount    = static_cast<UINT>(waterIndices.size()),
			    .toplogy       = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			} };
	    });
}