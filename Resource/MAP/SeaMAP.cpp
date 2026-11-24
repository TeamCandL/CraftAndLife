#include "pch/pch.h"
#include "SeaMAP.h"


#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>
#include <Game/Terrain/TerrainMaterial.h>
#include <Game/PlayScene/TerrainSystem.h>
#include <Game/PlayScene/Harvestable.h>
#include <Game/PlayScene/HarvestableController.h>
#include <Game/PlayScene/EntitySystem.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>


// ============================================================
// SeaMAP 内部設定
// ============================================================

namespace SeaMapConfig
{
	constexpr float COAST_FREQ_1 = 0.045f;
	constexpr float COAST_FREQ_2 = 0.110f;
	constexpr float EPSILON      = 0.0001f;
}  // namespace SeaMapConfig

// ============================================================
// 内部補助関数
// ============================================================

namespace
{
	float ClampValue(
	    float value,
	    float minValue,
	    float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	float Lerp(
	    float start,
	    float end,
	    float rate)
	{
		return start + (end - start) * rate;
	}

	float SmoothStep(
	    float rate)
	{
		rate = ClampValue(rate, 0.0f, 1.0f);
		return rate * rate * (3.0f - 2.0f * rate);
	}

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

	// 角度に応じた海岸線の半径（入り江・岬を作る）
	float GetCoastRadiusAtAngle(
	    float angle)
	{
		const float wave1 = std::sin(angle * 4.0f) * 8.0f + std::cos(angle * 7.0f + 0.8f) * 5.0f;
		const float wave2 = std::sin(angle * 13.0f + 2.1f) * 2.5f;
		return SeaMapConfig::ISLAND_BASE_RADIUS + wave1 + wave2;
	}

	// 疑似ノイズ（滑らかな起伏・海岸線の凹凸用）
	float PseudoNoise2D(
	    float x,
	    float z)
	{
		const float n1 = std::sin(x * SeaMapConfig::COAST_FREQ_1 + z * 0.028f) *
		                 std::cos(z * SeaMapConfig::COAST_FREQ_1 - x * 0.023f);
		const float n2 = std::sin(x * SeaMapConfig::COAST_FREQ_2 - z * 0.085f + 1.3f) *
		                 std::cos(z * SeaMapConfig::COAST_FREQ_2 + x * 0.075f - 0.7f);
		return n1 * 0.72f + n2 * 0.28f;
	}
}  // namespace

// ============================================================
// SeaMapData：初期化
// ============================================================

SeaMapData::SeaMapData(
    const GameObject _objHarvestableController) :
    objHarvestableController{ _objHarvestableController }
{
	Generate();
}

// ============================================================
// SeaMapData：配列の添字を計算する
// ============================================================

int32_t SeaMapData::GetIndex(
    int32_t x,
    int32_t z) const
{
	return z * (SeaMapConfig::MAP_WIDTH + 1) + x;
}

// ============================================================
// SeaMapData：マップの範囲内か判定する
// ============================================================

bool SeaMapData::IsInsideMap(
    float x,
    float z) const
{
	const float HALF_WIDTH = SeaMapConfig::MAP_WIDTH * SeaMapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = SeaMapConfig::MAP_DEPTH * SeaMapConfig::TILE_SIZE * 0.5f;

	return x >= -HALF_WIDTH && x < HALF_WIDTH && z >= -HALF_DEPTH && z < HALF_DEPTH;
}

// ============================================================
// SeaMapData：高さから地形種別を判定する
// ============================================================

SeaTerrainType SeaMapData::EvaluateTerrainType(
    float height) const
{
	if (height <= 2.0f)
	{
		return SeaTerrainType::Beach;
	}
	if (height >= 13.0f)
	{
		return SeaTerrainType::Mountain;
	}
	if (height >= 7.0f)
	{
		return SeaTerrainType::Highland;
	}
	return SeaTerrainType::Forest;
}

SeaTerrainType SeaMapData::GetTerrainType(
    float x,
    float z) const
{
	return EvaluateTerrainType(GetHeight(x, z));
}

// ============================================================
// SeaMapData：地形の高さを計算する（登りやすい傾斜・川/滝なし・滑らか）
// ============================================================

float SeaMapData::CalculateRawHeight(
    float x,
    float z) const
{
	// 初期位置（マップ中心 (0, 0)）を基準に本島を配置
	const float dx             = x - SeaMapConfig::ISLAND_CENTER_X;
	const float dz             = z - SeaMapConfig::ISLAND_CENTER_Z;
	const float distFromCenter = std::sqrt(dx * dx + dz * dz);
	const float angle          = std::atan2(dz, dx);

	const float noise       = PseudoNoise2D(x, z);
	const float coastRadius = GetCoastRadiusAtAngle(angle) + noise * SeaMapConfig::COAST_AMP_2;

	const float outerEdge = coastRadius + SeaMapConfig::COAST_AMP_1;
	if (distFromCenter >= outerEdge)
	{
		return SeaMapConfig::BASE_HEIGHT;
	}

	// 1. 海岸線から内陸への滑らかな立ち上がり
	const float innerCoast = coastRadius * 0.72f;
	const float denom      = std::max(outerEdge - innerCoast, SeaMapConfig::EPSILON);
	const float landT      = SmoothStep(1.0f - ClampValue((distFromCenter - innerCoast) / denom, 0.0f, 1.0f));

	float height = Lerp(SeaMapConfig::BASE_HEIGHT, SeaMapConfig::BEACH_HEIGHT + SeaMapConfig::LOWLAND_BASE, landT);

	// 2. 森林・高台のなだらかな起伏
	const float undulation = (noise * 3.5f + std::cos(x * 0.045f - z * 0.045f) * 1.5f) * landT;
	height += undulation;

	// 3. 中央の山岳（裾野を広げて角度を緩やかにし、登りやすくする）
	const float mDist = GetDistance(x, z, SeaMapConfig::MOUNTAIN_CENTER_X, SeaMapConfig::MOUNTAIN_CENTER_Z);
	if (mDist < SeaMapConfig::MOUNTAIN_RADIUS)
	{
		const float t              = ClampValue(mDist / SeaMapConfig::MOUNTAIN_RADIUS, 0.0f, 1.0f);
		const float mRate          = 1.0f - SmoothStep(SmoothStep(t));
		const float ridgeWave      = (std::cos(x * 0.06f + z * 0.05f) * 0.5f + 0.5f) * 1.5f + noise * 1.2f;
		const float mountainHeight = mRate * (SeaMapConfig::MOUNTAIN_PEAK + ridgeWave);
		height                     = std::max(height, mountainHeight);
	}

	return ClampValue(height, SeaMapConfig::BASE_HEIGHT, SeaMapConfig::MAX_HEIGHT);
}

// ============================================================
// SeaMapData：地形データを生成する
// ============================================================

void SeaMapData::Generate()
{
	const int32_t VERTEX_WIDTH = SeaMapConfig::MAP_WIDTH + 1;
	const int32_t VERTEX_DEPTH = SeaMapConfig::MAP_DEPTH + 1;

	const float HALF_WIDTH = SeaMapConfig::MAP_WIDTH * SeaMapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = SeaMapConfig::MAP_DEPTH * SeaMapConfig::TILE_SIZE * 0.5f;

	heights.resize(static_cast<size_t>(VERTEX_WIDTH) * static_cast<size_t>(VERTEX_DEPTH));
	terrainTypes.resize(static_cast<size_t>(SeaMapConfig::MAP_WIDTH) * static_cast<size_t>(SeaMapConfig::MAP_DEPTH));

	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X     = x * SeaMapConfig::TILE_SIZE - HALF_WIDTH;
			const float WORLD_Z     = z * SeaMapConfig::TILE_SIZE - HALF_DEPTH;
			heights[GetIndex(x, z)] = CalculateRawHeight(WORLD_X, WORLD_Z);
		}
	}

	for (int32_t z = 0; z < SeaMapConfig::MAP_DEPTH; ++z)
	{
		for (int32_t x = 0; x < SeaMapConfig::MAP_WIDTH; ++x)
		{
			const float CENTER_X = -HALF_WIDTH + (static_cast<float>(x) + 0.5f) * SeaMapConfig::TILE_SIZE;
			const float CENTER_Z = -HALF_DEPTH + (static_cast<float>(z) + 0.5f) * SeaMapConfig::TILE_SIZE;
			const size_t cellIdx =
			    static_cast<size_t>(z) * static_cast<size_t>(SeaMapConfig::MAP_WIDTH) + static_cast<size_t>(x);
			terrainTypes[cellIdx] = EvaluateTerrainType(GetHeight(CENTER_X, CENTER_Z));
		}
	}
}

// ============================================================
// SeaMapData：指定位置の地形の高さを取得する
// ============================================================

float SeaMapData::GetHeight(
    float x,
    float z) const
{
	if (heights.empty())
	{
		return SeaMapConfig::BASE_HEIGHT;
	}

	constexpr float HALF_SCALE     = 0.5f;
	constexpr float GRID_RATIO_MIN = 0.0f;
	constexpr float GRID_RATIO_MAX = 1.0f;

	const float HALF_WIDTH = static_cast<float>(SeaMapConfig::MAP_WIDTH) * SeaMapConfig::TILE_SIZE * HALF_SCALE;
	const float HALF_DEPTH = static_cast<float>(SeaMapConfig::MAP_DEPTH) * SeaMapConfig::TILE_SIZE * HALF_SCALE;

	const float GRID_X = ClampValue(
	    (x + HALF_WIDTH) / SeaMapConfig::TILE_SIZE,
	    GRID_RATIO_MIN,
	    static_cast<float>(SeaMapConfig::MAP_WIDTH));
	const float GRID_Z = ClampValue(
	    (z + HALF_DEPTH) / SeaMapConfig::TILE_SIZE,
	    GRID_RATIO_MIN,
	    static_cast<float>(SeaMapConfig::MAP_DEPTH));

	const int32_t X0 = static_cast<int32_t>(std::floor(GRID_X));
	const int32_t Z0 = static_cast<int32_t>(std::floor(GRID_Z));
	const int32_t X1 = std::min(X0 + 1, SeaMapConfig::MAP_WIDTH);
	const int32_t Z1 = std::min(Z0 + 1, SeaMapConfig::MAP_DEPTH);

	const float FX = GRID_X - static_cast<float>(X0);
	const float FZ = GRID_Z - static_cast<float>(Z0);

	const float H00 = heights[GetIndex(X0, Z0)];
	const float H10 = heights[GetIndex(X1, Z0)];
	const float H01 = heights[GetIndex(X0, Z1)];
	const float H11 = heights[GetIndex(X1, Z1)];

	constexpr float TRIANGLE_SPLIT_RATIO = 1.0f;
	if (FX + FZ <= TRIANGLE_SPLIT_RATIO)
	{
		return H00 + FX * (H10 - H00) + FZ * (H01 - H00);
	}
	return H10 * (GRID_RATIO_MAX - FZ) + H11 * (FX + FZ - TRIANGLE_SPLIT_RATIO) + H01 * (GRID_RATIO_MAX - FX);
}

bool SeaMapData::IsPath(
    float,
    float) const
{
	return false;
}
bool SeaMapData::IsRiver(
    float,
    float) const
{
	return false;
}

Vec3 SeaMapData::GetStartPosition() const
{
	return Vec3{ SeaMapConfig::START_X,
		         GetHeight(SeaMapConfig::START_X, SeaMapConfig::START_Z),
		         SeaMapConfig::START_Z };
}

Vec3 SeaMapData::GetGoalPosition() const
{
	return Vec3{ SeaMapConfig::GOAL_X, GetHeight(SeaMapConfig::GOAL_X, SeaMapConfig::GOAL_Z), SeaMapConfig::GOAL_Z };
}

Vec3 SeaMapData::GetBeginPosision() const
{
	return GetStartPosition();
}
Vec3 SeaMapData::GetEndPosision() const
{
	return GetGoalPosition();
}

// ============================================================
// SEAMAP：初期化（オブジェクト配置は後回し）
// ============================================================

void SEAMAP::Start()
{
	Get<TerrainSystem>().Register(this->ToGameObject());

	MaterialResource materialResource{};
	std::unique_ptr<TerrainMaterialProperty> property = std::make_unique<TerrainMaterialProperty>();
	property->Get().hasTexture                        = { TRUE, TRUE, TRUE, TRUE };
	property->Get().baseColor                         = { 1, 1, 1, 1 };

	materialResource.TryLoad(std::move(property));
	material = Resource<Material>{ std::move(materialResource) };

	Reload();

	// --------------------------------------------------------
	// 200x200のCSVからオブジェクトを配置する
	// 0:なし 1:Tree 2:TreeHigh 3:Rock
	// 4:Mushroom 5:Wood 6:Stone
	// --------------------------------------------------------
	LoadMapPlacementCSV();
}

// ============================================================
// MAP：200x200の配置CSVを読み込む
// ============================================================

void SEAMAP::LoadMapPlacementCSV()
{
	constexpr const char* CSV_FILE_NAME = "MapPlacement_island.csv";

	std::ifstream file(CSV_FILE_NAME);

	if (!file.is_open())
	{
		// CSVが開けない場合は配置せずに終了する
		return;
	}

	const float HALF_WIDTH = static_cast<float>(SeaMapConfig::MAP_WIDTH) * SeaMapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = static_cast<float>(SeaMapConfig::MAP_DEPTH) * SeaMapConfig::TILE_SIZE * 0.5f;

	std::string line{};
	int32_t gridZ = 0;

	while (std::getline(file, line))
	{
		if (gridZ >= SeaMapConfig::MAP_DEPTH)
		{
			break;
		}

		// 空行はマス目の行として数えない
		if (line.empty())
		{
			continue;
		}

		std::stringstream lineStream(line);
		std::string cell{};
		std::vector<int32_t> rowValues{};

		bool isValidRow = true;

		while (std::getline(lineStream, cell, ','))
		{
			try
			{
				size_t parsedLength = 0;
				const int32_t value = std::stoi(cell, &parsedLength);

				// 数字以外が混ざっているセルは無効
				if (parsedLength != cell.size() || value < 0 || value > 6)
				{
					isValidRow = false;
					break;
				}

				rowValues.push_back(value);
			}
			catch (const std::exception&)
			{
				isValidRow = false;
				break;
			}
		}

		// 1行が200列でなければ、その行は配置しない
		if (!isValidRow || rowValues.size() != static_cast<size_t>(SeaMapConfig::MAP_WIDTH))
		{
			++gridZ;
			continue;
		}

		for (int32_t gridX = 0; gridX < SeaMapConfig::MAP_WIDTH; ++gridX)
		{
			const int32_t type = rowValues[static_cast<size_t>(gridX)];

			// 0は何も配置しない
			if (type == 0)
			{
				continue;
			}

			// 各マスの中央を配置座標にする
			const float worldX = -HALF_WIDTH + (static_cast<float>(gridX) + 0.5f) * SeaMapConfig::TILE_SIZE;

			const float worldZ = -HALF_DEPTH + (static_cast<float>(gridZ) + 0.5f) * SeaMapConfig::TILE_SIZE;


			PlaceMapObject(type, worldX, worldZ);
		}

		++gridZ;
	}
}  // ============================================================
// MAP：数字に対応したオブジェクトを配置する
// ============================================================

void SEAMAP::PlaceMapObject(
    int32_t type,
    float x,
    float z)
{
	const float groundHeight = GetHeight(x, z);
	const Vec3 position{ x, groundHeight, z };

	switch (type)
	{
	case 1:  // Tree
		Get<HarvestableController>(objHarvestableController).AddHarvestable(HarvestableType::Tree, position, 5.0f);
		break;

	case 2:  // TreeHigh
		Get<HarvestableController>(objHarvestableController).AddHarvestable(HarvestableType::TreeHigh, position, 5.2f);
		break;

	case 3:  // Rock
		Get<HarvestableController>(objHarvestableController).AddHarvestable(HarvestableType::Rock, position, 3.0f);
		break;

	case 4:  // Mushroom
		Get<HarvestableController>(objHarvestableController).AddHarvestable(HarvestableType::Mushroom, position, 0.5f);
		break;

	case 5:  // Wood（地面アイテム）
		Get<EntitySystem>().Spawn(8, position, Vec3());
		break;

	case 6:  // Stone（地面アイテム）
		Get<EntitySystem>().Spawn(7, position, Vec3());
		break;

	default:
		break;
	}
}
void SEAMAP::Update() {}

// ============================================================
// SEAMAP：地形を描画する
// ============================================================

void SEAMAP::Draw()
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
// SEAMAP：頂点同士を結んだ滑らかなメッシュを生成する
// ============================================================

void SEAMAP::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& device)
	    {
		    using Vertex = ModelVertex;

		    const uint32_t VERTEX_WIDTH = SeaMapConfig::MAP_WIDTH + 1;
		    const uint32_t VERTEX_DEPTH = SeaMapConfig::MAP_DEPTH + 1;
		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

		    std::vector<Vertex> vertices(VERTEX_COUNT);

		    const float HALF_WIDTH = SeaMapConfig::MAP_WIDTH * SeaMapConfig::TILE_SIZE * 0.5f;
		    const float HALF_DEPTH = SeaMapConfig::MAP_DEPTH * SeaMapConfig::TILE_SIZE * 0.5f;

		    // 1. 頂点位置・UV・滑らかな法線を計算
		    for (uint32_t z = 0; z < VERTEX_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH; ++x)
			    {
				    const uint32_t INDEX = x + z * VERTEX_WIDTH;

				    Vec3 position{};
				    position.x = x * SeaMapConfig::TILE_SIZE - HALF_WIDTH;
				    position.z = z * SeaMapConfig::TILE_SIZE - HALF_DEPTH;
				    position.y = heights[GetIndex(static_cast<int32_t>(x), static_cast<int32_t>(z))];

				    vertices[INDEX].position = position;
				    vertices[INDEX].uv       = Vec2{ x * SeaMapConfig::TILE_SIZE, z * SeaMapConfig::TILE_SIZE };

				    const float LEFT_HEIGHT  = GetHeight(position.x - SeaMapConfig::TILE_SIZE, position.z);
				    const float RIGHT_HEIGHT = GetHeight(position.x + SeaMapConfig::TILE_SIZE, position.z);
				    const float BACK_HEIGHT  = GetHeight(position.x, position.z - SeaMapConfig::TILE_SIZE);
				    const float FRONT_HEIGHT = GetHeight(position.x, position.z + SeaMapConfig::TILE_SIZE);

				    const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT,
					                   2.0f * SeaMapConfig::TILE_SIZE,
					                   BACK_HEIGHT - FRONT_HEIGHT };
				    const float NORMAL_LENGTH =
				        std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

				    if (NORMAL_LENGTH > SeaMapConfig::EPSILON)
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

		    // 2. 隣り合う頂点を三角形2枚で結ぶ
		    const uint32_t INDEX_COUNT = SeaMapConfig::MAP_WIDTH * SeaMapConfig::MAP_DEPTH * 6;
		    std::vector<uint32_t> indices(INDEX_COUNT);

		    uint32_t index = 0;
		    for (uint32_t z = 0; z < SeaMapConfig::MAP_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < SeaMapConfig::MAP_WIDTH; ++x)
			    {
				    const uint32_t TOP_LEFT     = z * VERTEX_WIDTH + x;
				    const uint32_t TOP_RIGHT    = TOP_LEFT + 1;
				    const uint32_t BOTTOM_LEFT  = (z + 1) * VERTEX_WIDTH + x;
				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    indices[index++] = TOP_LEFT;
				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_LEFT;

				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_RIGHT;
				    indices[index++] = BOTTOM_LEFT;
			    }
		    }

		    const D3D11_BUFFER_DESC VB_DESC{ .ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_VERTEX_BUFFER };

		    ComPtr<ID3D11Buffer> vertexBuffer{};
		    const D3D11_SUBRESOURCE_DATA VB_DATA{ .pSysMem = vertices.data() };
		    if (FAILED(device->CreateBuffer(&VB_DESC, &VB_DATA, vertexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    const D3D11_BUFFER_DESC IB_DESC{ .ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_INDEX_BUFFER };

		    ComPtr<ID3D11Buffer> indexBuffer{};
		    const D3D11_SUBRESOURCE_DATA IB_DATA{ .pSysMem = indices.data() };
		    if (FAILED(device->CreateBuffer(&IB_DESC, &IB_DATA, indexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    surface = Resource<Surface>{ SurfaceResource{ .pVertexBuffer = vertexBuffer,
			                                              .pIndexBuffer  = indexBuffer,
			                                              .size          = static_cast<UINT>(sizeof(Vertex)),
			                                              .offset        = 0,
			                                              .vertexCount   = static_cast<UINT>(vertices.size()),
			                                              .indexCount    = static_cast<UINT>(indices.size()),
			                                              .toplogy       = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST } };
	    });
}

// ============================================================
// ITerrain 実装
// ============================================================

float SEAMAP::GetHeight(
    const float worldX,
    const float worldZ)
{
	if (heights.empty())
	{
		return SeaMapConfig::BASE_HEIGHT;
	}
	return SeaMapData::GetHeight(worldX, worldZ);
}

Vec3 SEAMAP::GetNormal(
    const float worldX,
    const float worldZ)
{
	if (heights.empty())
	{
		return Vec3::Up();
	}

	constexpr float SAMPLE_DISTANCE = SeaMapConfig::TILE_SIZE;
	const float LEFT_HEIGHT         = SeaMapData::GetHeight(worldX - SAMPLE_DISTANCE, worldZ);
	const float RIGHT_HEIGHT        = SeaMapData::GetHeight(worldX + SAMPLE_DISTANCE, worldZ);
	const float BACK_HEIGHT         = SeaMapData::GetHeight(worldX, worldZ - SAMPLE_DISTANCE);
	const float FRONT_HEIGHT        = SeaMapData::GetHeight(worldX, worldZ + SAMPLE_DISTANCE);

	const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT, 2.0f * SAMPLE_DISTANCE, BACK_HEIGHT - FRONT_HEIGHT };
	const float NORMAL_LENGTH = std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

	if (NORMAL_LENGTH <= SeaMapConfig::EPSILON)
	{
		return Vec3::Up();
	}

	return Vec3{ NORMAL.x / NORMAL_LENGTH, NORMAL.y / NORMAL_LENGTH, NORMAL.z / NORMAL_LENGTH };
}

Vec3 SEAMAP::GetSize()
{
	return Vec3{ static_cast<float>(SeaMapConfig::MAP_WIDTH),
		         SeaMapConfig::MAX_HEIGHT,
		         static_cast<float>(SeaMapConfig::MAP_DEPTH) };
}

Vec3 SEAMAP::GetMinPosition()
{
	const float HALF_WIDTH = SeaMapConfig::MAP_WIDTH * SeaMapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = SeaMapConfig::MAP_DEPTH * SeaMapConfig::TILE_SIZE * 0.5f;
	return Vec3{ -HALF_WIDTH, SeaMapConfig::BASE_HEIGHT, -HALF_DEPTH };
}

Vec3 SEAMAP::GetMaxPosition()
{
	const float HALF_WIDTH = SeaMapConfig::MAP_WIDTH * SeaMapConfig::TILE_SIZE * 0.5f;
	const float HALF_DEPTH = SeaMapConfig::MAP_DEPTH * SeaMapConfig::TILE_SIZE * 0.5f;
	return Vec3{ HALF_WIDTH, SeaMapConfig::MAX_HEIGHT, HALF_DEPTH };
}