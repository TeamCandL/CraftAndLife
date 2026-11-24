
#include "pch/pch.h"
#include "SEAMAP_Island.h"
#include "SeaMAP.h"

#include <Game/Terrain/TerrainMaterial.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>

// ============================================================KO
// ============================================================

namespace
{
	// 0～1の範囲に制限する
	float ClampValue(
	    float value,
	    float minValue,
	    float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	// 滑らかな補間
	float SmoothStep(
	    float value)
	{
		value = ClampValue(value, 0.0f, 1.0f);

		return value * value * (3.0f - 2.0f * value);
	}

	// 2点間の距離
	float GetDistance(
	    float x1,
	    float z1,
	    float x2,
	    float z2)
	{
		const float DX = x1 - x2;
		const float DZ = z1 - z2;

		return std::sqrt(DX * DX + DZ * DZ);
	}

	// 線形補間
	float Lerp(
	    float start,
	    float end,
	    float rate)
	{
		return start + (end - start) * rate;
	}
}  // namespace

// ============================================================
// MAP_IslandData：コンストラクタ
// ============================================================

SEAMAP_IslandData::SEAMAP_IslandData(
    GameObject mainMap) :
    objMainMap{ mainMap }
{
}

// ============================================================
// MAP_IslandData：配列の添字
// ============================================================

int32_t SEAMAP_IslandData::GetIndex(
    int32_t x,
    int32_t z) const
{
	return z * IslandConfig::SEAVERTEX_WIDTH + x;
}

// ============================================================
// MAP_IslandData：砂浜の地形生成
// ============================================================

void SEAMAP_IslandData::Generate(
    SEAMAP& mainMap)
{
	const int32_t VERTEX_WIDTH = IslandConfig::SEAVERTEX_WIDTH;
	const int32_t VERTEX_DEPTH = IslandConfig::SEAVERTEX_DEPTH;

	heights.resize(static_cast<size_t>(VERTEX_WIDTH) * static_cast<size_t>(VERTEX_DEPTH));

	// 砂浜全体の頂点を生成
	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			// ワールド座標
			const float WORLD_X = x * IslandConfig::SEATILE_SIZE - (IslandConfig::SEAHALF_WIDTH + IslandConfig::SEABEACH_WIDTH);

			const float WORLD_Z = z * IslandConfig::SEATILE_SIZE - (IslandConfig::SEAHALF_DEPTH + IslandConfig::SEABEACH_WIDTH);

			// メインマップの範囲内に収めた座標
			const float MAIN_X = ClampValue(WORLD_X, -IslandConfig::SEAHALF_WIDTH, IslandConfig::SEAHALF_WIDTH);

			const float MAIN_Z = ClampValue(WORLD_Z, -IslandConfig::SEAHALF_DEPTH, IslandConfig::SEAHALF_DEPTH);

			// メインマップの外周の高さを取得
			const float MAIN_HEIGHT = mainMap.GetHeight(MAIN_X, MAIN_Z);

			// メインマップの外側までの距離
			const float DISTANCE_X = std::max(0.0f, std::abs(WORLD_X) - IslandConfig::SEAHALF_WIDTH);

			const float DISTANCE_Z = std::max(0.0f, std::abs(WORLD_Z) - IslandConfig::SEAHALF_DEPTH);

			// 外周からの距離
			const float DISTANCE = std::sqrt(DISTANCE_X * DISTANCE_X + DISTANCE_Z * DISTANCE_Z);

			// 砂浜の外側に向かう割合
			const float RATE = ClampValue(DISTANCE / IslandConfig::SEABEACH_WIDTH, 0.0f, 1.0f);

			// 滑らかに下がる割合
			const float BLEND_RATE = SmoothStep(RATE);

			// 外側に向かって徐々に低くする
			const float BEACH_HEIGHT = MAIN_HEIGHT - IslandConfig::SEABEACH_DROP * BLEND_RATE;

			heights[GetIndex(x, z)] = BEACH_HEIGHT;
		}
	}
}

// ============================================================
// MAP_Island：初期化
// ============================================================

void SEAMAP_Island::Start()
{
	// メインマップを取得
	SEAMAP& mainMap = Get<SEAMAP>(objMainMap);

	// 砂浜の地形データを生成
	Generate(mainMap);

	// マテリアルを作成
	MaterialResource materialResource{};

	std::unique_ptr<TerrainMaterialProperty> property = std::make_unique<TerrainMaterialProperty>();

	property->Get().hasTexture = { TRUE, TRUE, TRUE, TRUE };

	property->Get().baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialResource.TryLoad(std::move(property));

	material = Resource<Material>{ std::move(materialResource) };

	// メッシュ生成
	Reload();
}

// ============================================================
// MAP_Island：更新
// ============================================================

void SEAMAP_Island::Update() {}

// ============================================================
// SEAMAP_Island：描画
// ============================================================

void SEAMAP_Island::Draw()
{
	Dx11Manager& dx11 = Get<Dx11Manager>();

	dx11.SetMaterial(transform.GetMaterial());
	dx11.SetMaterial(material);
	dx11.SetShader(shader);
	dx11.SetSurface(surface);
	dx11.SetTexture(texture);

	dx11.Draw();
}

// ============================================================
// MAP_Island：砂浜メッシュ生成
// ============================================================

void SEAMAP_Island::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& device)
	    {
		    using Vertex = ModelVertex;

		    // ------------------------------------------------
		    // 1. 頂点数
		    // ------------------------------------------------

		    const uint32_t VERTEX_WIDTH = IslandConfig::SEAVERTEX_WIDTH;

		    const uint32_t VERTEX_DEPTH = IslandConfig::SEAVERTEX_DEPTH;

		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

		    std::vector<Vertex> vertices(VERTEX_COUNT);

		    // 砂浜全体の半分の大きさ
		    const float HALF_WIDTH = IslandConfig::SEAHALF_WIDTH + IslandConfig::SEABEACH_WIDTH;

		    const float HALF_DEPTH = IslandConfig::SEAHALF_DEPTH + IslandConfig::SEABEACH_WIDTH;

		    // ------------------------------------------------
		    // 2. 頂点の作成
		    // ------------------------------------------------

		    for (uint32_t z = 0; z < VERTEX_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH; ++x)
			    {
				    const uint32_t INDEX = x + z * VERTEX_WIDTH;

				    Vec3 position{};

				    position.x = x * IslandConfig::SEATILE_SIZE - HALF_WIDTH;

				    position.y = heights[GetIndex(static_cast<int32_t>(x), static_cast<int32_t>(z))];

				    position.z = z * IslandConfig::SEATILE_SIZE - HALF_DEPTH;

				    vertices[INDEX].position = position;

				    // テクスチャの繰り返し
				    vertices[INDEX].uv = Vec2{ x * IslandConfig::SEATILE_SIZE, z * IslandConfig::SEATILE_SIZE };

				    // 周囲の高さから法線を計算
				    const int32_t SAMPLE_X = static_cast<int32_t>(x);

				    const int32_t SAMPLE_Z = static_cast<int32_t>(z);

				    const int32_t LEFT_X = std::max(0, SAMPLE_X - 1);

				    const int32_t RIGHT_X = std::min(static_cast<int32_t>(VERTEX_WIDTH) - 1, SAMPLE_X + 1);

				    const int32_t BACK_Z = std::max(0, SAMPLE_Z - 1);

				    const int32_t FRONT_Z = std::min(static_cast<int32_t>(VERTEX_DEPTH) - 1, SAMPLE_Z + 1);

				    const float LEFT_HEIGHT = heights[GetIndex(LEFT_X, SAMPLE_Z)];

				    const float RIGHT_HEIGHT = heights[GetIndex(RIGHT_X, SAMPLE_Z)];

				    const float BACK_HEIGHT = heights[GetIndex(SAMPLE_X, BACK_Z)];

				    const float FRONT_HEIGHT = heights[GetIndex(SAMPLE_X, FRONT_Z)];

				    const float DISTANCE_X = (RIGHT_X - LEFT_X) * IslandConfig::SEATILE_SIZE;

				    const float DISTANCE_Z = (FRONT_Z - BACK_Z) * IslandConfig::SEATILE_SIZE;

				    const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT,
					                   std::max(DISTANCE_X + DISTANCE_Z, IslandConfig::SEAEPSILON),
					                   BACK_HEIGHT - FRONT_HEIGHT };

				    const float NORMAL_LENGTH =
				        std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

				    if (NORMAL_LENGTH > IslandConfig::SEAEPSILON)
				    {
					    vertices[INDEX].normal =
					        Vec3{ NORMAL.x / NORMAL_LENGTH, NORMAL.y / NORMAL_LENGTH, NORMAL.z / NORMAL_LENGTH };
				    }
				    else
				    {
					    vertices[INDEX].normal = Vec3::Up();
				    }
			    }
		    }

		    // ------------------------------------------------
		    // 3. インデックスの作成
		    // ------------------------------------------------

		    std::vector<uint32_t> indices{};

		    const int32_t INNER_START = IslandConfig::SEAINNER_START;

		    const int32_t INNER_END_X = INNER_START + IslandConfig::SEAMAIN_VERTEX_WIDTH - 1;

		    const int32_t INNER_END_Z = INNER_START + IslandConfig::SEAMAIN_VERTEX_DEPTH - 1;

		    for (uint32_t z = 0; z < VERTEX_DEPTH - 1; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH - 1; ++x)
			    {
				    const int32_t GRID_X = static_cast<int32_t>(x);

				    const int32_t GRID_Z = static_cast<int32_t>(z);

				    // メインマップの内側は生成しない
				    const bool IS_INSIDE_MAIN_MAP =
				        GRID_X >= INNER_START && GRID_X < INNER_END_X && GRID_Z >= INNER_START && GRID_Z < INNER_END_Z;

				    if (IS_INSIDE_MAIN_MAP)
				    {
					    continue;
				    }

				    const uint32_t TOP_LEFT = z * VERTEX_WIDTH + x;

				    const uint32_t TOP_RIGHT = TOP_LEFT + 1;

				    const uint32_t BOTTOM_LEFT = (z + 1) * VERTEX_WIDTH + x;

				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    // 上向きの三角形1
				    indices.push_back(TOP_LEFT);
				    indices.push_back(BOTTOM_LEFT);
				    indices.push_back(TOP_RIGHT);

				    // 上向きの三角形2
				    indices.push_back(TOP_RIGHT);
				    indices.push_back(BOTTOM_LEFT);
				    indices.push_back(BOTTOM_RIGHT);
			    }
		    }

		    // インデックスが空なら終了
		    if (indices.empty())
		    {
			    return;
		    }

		    // ------------------------------------------------
		    // 4. 頂点バッファ
		    // ------------------------------------------------

		    const D3D11_BUFFER_DESC VB_DESC{ .ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_VERTEX_BUFFER };

		    ComPtr<ID3D11Buffer> vertexBuffer{};

		    const D3D11_SUBRESOURCE_DATA VB_DATA{ .pSysMem = vertices.data() };

		    if (FAILED(device->CreateBuffer(&VB_DESC, &VB_DATA, vertexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    // ------------------------------------------------
		    // 5. インデックスバッファ
		    // ------------------------------------------------

		    const D3D11_BUFFER_DESC IB_DESC{ .ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_INDEX_BUFFER };

		    ComPtr<ID3D11Buffer> indexBuffer{};

		    const D3D11_SUBRESOURCE_DATA IB_DATA{ .pSysMem = indices.data() };

		    if (FAILED(device->CreateBuffer(&IB_DESC, &IB_DATA, indexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    // ------------------------------------------------
		    // 6. Surface登録
		    // ------------------------------------------------

		    surface = Resource<Surface>{ SurfaceResource{ .pVertexBuffer = vertexBuffer,
			                                              .pIndexBuffer  = indexBuffer,
			                                              .size          = static_cast<UINT>(sizeof(Vertex)),
			                                              .offset        = 0,
			                                              .vertexCount   = static_cast<UINT>(vertices.size()),
			                                              .indexCount    = static_cast<UINT>(indices.size()),
			                                              .toplogy       = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST } };
	    });
}
