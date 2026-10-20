
#include "MapTestScene.h"
#include "pch\pch.h"
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include "..//Terrain/TerrainMaterial.h"

// ============================================================
// 共通処理
// ============================================================

namespace
{
	constexpr float PI = 3.14159265358979323846f;

	// 道の中心位置
	constexpr float PATH_START_X = -70.0f;
	constexpr float PATH_END_X = 70.0f;

	// 川の位置
	constexpr float RIVER_CENTER_Z = 0.0f;

	float ClampValue(const float value, const float minValue, const float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	float Lerp(const float a, const float b, const float t)
	{
		return a + (b - a) * t;
	}

	float SmoothStep(float t)
	{
		t = ClampValue(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
} // namespace

// ============================================================
// MapTestScene
// ============================================================

void MapTestScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<Sky>();

	// マップだけを生成する
	Instantiate<MAP>();
}

// ============================================================
// MapData
// ============================================================

MapData::MapData()
{
	Generate();
}

int32_t MapData::GetIndex(const int32_t x, const int32_t z) const
{
	return z * (MapConfig::MAP_WIDTH + 1) + x;
}

// ============================================================
// マップ範囲判定
// ============================================================

bool MapData::IsInsideMap(const float x, const float z) const
{
	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	return x >= -HALF_WIDTH && x < HALF_WIDTH && z >= -HALF_DEPTH && z < HALF_DEPTH;
}

// ============================================================
// 地形の高さ計算
// ============================================================

float MapData::CalculateHeight(const float x, const float z) const
{
	// 大きな丘
	const float HILL_A = std::sin(x * 0.035f) * 3.0f;
	const float HILL_B = std::cos(z * 0.045f) * 2.0f;
	const float HILL_C = std::sin((x + z) * 0.025f) * 2.0f;

	float height = MapConfig::BASE_HEIGHT + HILL_A + HILL_B + HILL_C;

	// マップ中央付近に高低差を作る
	const float DISTANCE_FROM_CENTER = std::sqrt(x * x + z * z);

	height += std::max(0.0f, 1.0f - DISTANCE_FROM_CENTER / 70.0f) * 3.0f;

	// 川の部分を低くする
	const float RIVER_DISTANCE = std::abs(z - RIVER_CENTER_Z);

	if (RIVER_DISTANCE < MapConfig::RIVER_WIDTH * 0.5f)
	{
		height -= 2.0f;
	}

	return ClampValue(height, MapConfig::BASE_HEIGHT, MapConfig::MAX_HEIGHT);
}

// ============================================================
// 地形生成
// ============================================================

void MapData::Generate()
{
	// 200×200の地形に必要な頂点数は201×201
	const int32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;
	const int32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

	heights.resize(VERTEX_WIDTH * VERTEX_DEPTH);

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	// 全頂点の高さを計算
	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			heights[GetIndex(x, z)] = CalculateHeight(WORLD_X, WORLD_Z);
		}
	}

	// 地形の高低差を滑らかにする
	SmoothHeights();

	// 道の部分は歩きやすい高さにする
	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			if (IsPath(WORLD_X, WORLD_Z))
			{
				heights[GetIndex(x, z)] = MapConfig::BASE_HEIGHT;
			}
		}
	}
}

// ============================================================
// 高さの平滑化
// ============================================================

void MapData::SmoothHeights()
{
	std::vector<float> temporary(heights.size());

	for (int32_t iteration = 0; iteration < MapConfig::SMOOTHING_ITERATIONS; ++iteration)
	{
		temporary = heights;

		for (int32_t z = 1; z < MapConfig::MAP_DEPTH; ++z)
		{
			for (int32_t x = 1; x < MapConfig::MAP_WIDTH; ++x)
			{
				const int32_t INDEX = GetIndex(x, z);

				// 上下左右の平均
				const float NEIGHBOR_AVERAGE = (heights[GetIndex(x - 1, z)] + heights[GetIndex(x + 1, z)] +
				                                heights[GetIndex(x, z - 1)] + heights[GetIndex(x, z + 1)]) *
				                               0.25f;

				// 元の高さと周囲の平均を補間する
				temporary[INDEX] = Lerp(heights[INDEX], NEIGHBOR_AVERAGE, MapConfig::SMOOTHING_RATE);
			}
		}

		heights.swap(temporary);
	}
}

// ============================================================
// 高さ取得
// ============================================================

float MapData::GetHeight(const float x, const float z) const
{
	if (!IsInsideMap(x, z) || heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	const float GRID_X = (x + HALF_WIDTH) / MapConfig::TILE_SIZE;

	const float GRID_Z = (z + HALF_DEPTH) / MapConfig::TILE_SIZE;

	const int32_t X0 = static_cast<int32_t>(GRID_X);
	const int32_t Z0 = static_cast<int32_t>(GRID_Z);

	const int32_t X1 = std::min(X0 + 1, MapConfig::MAP_WIDTH);
	const int32_t Z1 = std::min(Z0 + 1, MapConfig::MAP_DEPTH);

	const float TX = GRID_X - X0;
	const float TZ = GRID_Z - Z0;

	const float H00 = heights[GetIndex(X0, Z0)];
	const float H10 = heights[GetIndex(X1, Z0)];
	const float H01 = heights[GetIndex(X0, Z1)];
	const float H11 = heights[GetIndex(X1, Z1)];

	// X方向に補間
	const float TOP = Lerp(H00, H10, TX);
	const float BOTTOM = Lerp(H01, H11, TX);

	// Z方向に補間
	return Lerp(TOP, BOTTOM, TZ);
}

// ============================================================
// 道・川
// ============================================================

bool MapData::IsPath(const float x, const float z) const
{
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	const float PATH_RATE = ClampValue((z + HALF_DEPTH) / (MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE), 0.0f, 1.0f);

	const float PATH_CENTER_X = Lerp(PATH_START_X, PATH_END_X, PATH_RATE);

	return std::abs(x - PATH_CENTER_X) <= MapConfig::PATH_WIDTH * 0.5f;
}

bool MapData::IsRiver(const float x, const float z) const
{
	(void)x;

	return std::abs(z - RIVER_CENTER_Z) <= MapConfig::RIVER_WIDTH * 0.5f;
}

// ============================================================
// スタート・ゴール
// ============================================================

Vec3 MapData::GetStartPosition() const
{
	const float X = PATH_START_X;
	const float Z = -MapConfig::MAP_DEPTH * 0.4f;

	return Vec3{X, GetHeight(X, Z), Z};
}

Vec3 MapData::GetGoalPosition() const
{
	const float X = PATH_END_X;
	const float Z = MapConfig::MAP_DEPTH * 0.4f;

	return Vec3{X, GetHeight(X, Z), Z};
}

// ============================================================
// MAP
// ============================================================

void MAP::Start()
{
#pragma region マテリアル作成
	MaterialResource material{};

	std::unique_ptr<TerrainMaterialProperty> pModelProperty = std::make_unique<TerrainMaterialProperty>();
	pModelProperty->Get().hasTexture = {TRUE, TRUE, TRUE, TRUE};
	pModelProperty->Get().baseColor = {1, 1, 1, 1};

	material.TryLoad(std::move(pModelProperty));

	this->material = Resource<Material>{std::move(material)};
#pragma endregion

	// 高さデータから地形メッシュを作成
	Reload();
}

void MAP::Update()
{
	// 現段階では更新処理なし
}

void MAP::Draw()
{
	Dx11Manager& DX11 = Get<Dx11Manager>();

	DX11.SetMaterial(transform.GetMaterial());
	DX11.SetMaterial(material);

	DX11.SetShader(shader);
	DX11.SetSurface(surface);
	DX11.SetTexture(texture);

	DX11.Draw();
}

// ============================================================
// 地形メッシュ作成
// ============================================================

void MAP::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& _pDevice)
	    {
		    using Vertex = ModelVertex;

		    const uint32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;

		    const uint32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

#pragma region 頂点作成

		    std::vector<Vertex> vertices(VERTEX_COUNT);

		    const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

		    const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

		    for (uint32_t z = 0; z < VERTEX_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH; ++x)
			    {
				    const uint32_t INDEX = x + z * VERTEX_WIDTH;

				    Vec3 position{};

				    position.x = x * MapConfig::TILE_SIZE - HALF_WIDTH;

				    position.z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

				    position.y = heights[GetIndex(static_cast<int32_t>(x), static_cast<int32_t>(z))];

				    vertices[INDEX].position = position;

				    vertices[INDEX].uv = Vec2{x * MapConfig::TILE_SIZE, z * MapConfig::TILE_SIZE};

				    vertices[INDEX].normal = Vec3::Up();
			    }
		    }

#pragma endregion

#pragma region インデックス作成

		    // 1タイルにつき三角形2つ、インデックス6個
		    const uint32_t INDEX_COUNT = MapConfig::MAP_WIDTH * MapConfig::MAP_DEPTH * 6;

		    std::vector<uint32_t> indices(INDEX_COUNT);

		    uint32_t indicesCount = 0;

		    for (uint32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
			    {
				    const uint32_t TOP_LEFT = z * VERTEX_WIDTH + x;

				    const uint32_t TOP_RIGHT = TOP_LEFT + 1;

				    const uint32_t BOTTOM_LEFT = (z + 1) * VERTEX_WIDTH + x;

				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    // 三角形1
				    indices[indicesCount++] = TOP_LEFT;
				    indices[indicesCount++] = TOP_RIGHT;
				    indices[indicesCount++] = BOTTOM_LEFT;

				    // 三角形2
				    indices[indicesCount++] = TOP_RIGHT;
				    indices[indicesCount++] = BOTTOM_RIGHT;
				    indices[indicesCount++] = BOTTOM_LEFT;
			    }
		    }

#pragma endregion

#pragma region 頂点法線計算

		    // 各三角形の法線を頂点に加算
		    for (size_t i = 0; i < indices.size(); i += 3)
		    {
			    const uint32_t I0 = indices[i];
			    const uint32_t I1 = indices[i + 1];
			    const uint32_t I2 = indices[i + 2];

			    const Vec3 V0 = vertices[I0].position.v;
			    const Vec3 V1 = vertices[I1].position.v;
			    const Vec3 V2 = vertices[I2].position.v;

			    const Vec3 EDGE1 = V1 - V0;
			    const Vec3 EDGE2 = V2 - V0;

			    const Vec3 FACE_NORMAL = DirectX::XMVector3Cross(EDGE1, EDGE2);

			    vertices[I0].normal = Vec3{vertices[I0].normal.v} + FACE_NORMAL;

			    vertices[I1].normal = Vec3{vertices[I1].normal.v} + FACE_NORMAL;

			    vertices[I2].normal = Vec3{vertices[I2].normal.v} + FACE_NORMAL;
		    }

		    // 法線を正規化
		    for (Vertex& vertex : vertices)
		    {
			    Vec3 normal{vertex.normal.v};

			    normal = DirectX::XMVector3Normalize(normal);

			    vertex.normal = normal;
		    }

#pragma endregion

#pragma region 頂点バッファ作成

		    const D3D11_BUFFER_DESC VERTEX_BUFFER_DESC{
		        .ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
		        .Usage = D3D11_USAGE_DEFAULT,
		        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
		        .CPUAccessFlags = NULL,
		        .MiscFlags = NULL,
		        .StructureByteStride = 0,
		    };

		    ComPtr<ID3D11Buffer> pVertexBuffer{};

		    const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_VERTEX{
		        .pSysMem = vertices.data(),
		        .SysMemPitch = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
		        .SysMemSlicePitch = 0,
		    };

		    _pDevice->CreateBuffer(&VERTEX_BUFFER_DESC, &INITIALIZE_DATA_VERTEX, pVertexBuffer.GetAddressOf());

#pragma endregion

#pragma region インデックスバッファ作成

		    const D3D11_BUFFER_DESC INDEX_BUFFER_DESC{
		        .ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
		        .Usage = D3D11_USAGE_DEFAULT,
		        .BindFlags = D3D11_BIND_INDEX_BUFFER,
		        .CPUAccessFlags = NULL,
		        .MiscFlags = NULL,
		        .StructureByteStride = 0,
		    };

		    ComPtr<ID3D11Buffer> pIndexBuffer{};

		    const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_INDEX{
		        .pSysMem = indices.data(),
		        .SysMemPitch = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
		        .SysMemSlicePitch = 0,
		    };

		    _pDevice->CreateBuffer(&INDEX_BUFFER_DESC, &INITIALIZE_DATA_INDEX, pIndexBuffer.GetAddressOf());

#pragma endregion

#pragma region サーフェイス作成

		    SurfaceResource surface{
		        .pVertexBuffer = pVertexBuffer,
		        .pIndexBuffer = pIndexBuffer,
		        .size = static_cast<UINT>(sizeof(Vertex)),
		        .offset = 0,
		        .vertexCount = static_cast<UINT>(vertices.size()),
		        .indexCount = static_cast<UINT>(indices.size()),
		        .toplogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		    };

		    this->surface = {std::move(surface)};

#pragma endregion
	    });
}
