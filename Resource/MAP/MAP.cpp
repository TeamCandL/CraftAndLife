
#include "pch/pch.h"
#include "MAP.h"

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

// ============================================================
// マップ内部設定
// MAP.h にある MapConfig はマップ全体の設定です。
// ここには MAP.cpp 内部でのみ使用する調整値をまとめます。
// ============================================================

namespace MapConfig
{
	// 地形の細かな起伏
	constexpr float TERRAIN_WAVE_X_FREQUENCY = 0.03f;
	constexpr float TERRAIN_WAVE_Z_FREQUENCY = 0.035f;
	constexpr float TERRAIN_WAVE_HEIGHT      = 1.2f;

	// 渦巻き丘の内部形状
	constexpr float SPIRAL_RIDGE_HEIGHT       = 2.0f;
	constexpr float SPIRAL_CENTER_BASIN_DEPTH = 10.0f;

	// 渦巻き丘の入口
	constexpr float ENTRANCE_WIDTH  = 10.0f;
	constexpr float ENTRANCE_LENGTH = 40.0f;
	constexpr float ENTRANCE_HEIGHT = 5.0f;

	// 渦巻き丘の内部の床
	constexpr float BACK_FLOOR_WIDTH  = 24.0f;
	constexpr float BACK_FLOOR_LENGTH = 10.0f;
	constexpr float BACK_FLOOR_HEIGHT = 3.0f;
	constexpr float BACK_FLOOR_BLEND  = 20.0f;

	// 左右の段丘
	constexpr float LEFT_TERRACE_X      = -12.0f;
	constexpr float LEFT_TERRACE_Z      = 5.0f;
	constexpr float LEFT_TERRACE_RADIUS = 24.0f;
	constexpr float LEFT_TERRACE_HEIGHT = 2.5f;

	constexpr float RIGHT_TERRACE_X      = 35.0f;
	constexpr float RIGHT_TERRACE_Z      = 22.0f;
	constexpr float RIGHT_TERRACE_RADIUS = 25.0f;
	constexpr float RIGHT_TERRACE_HEIGHT = 3.0f;

	// 北東の山
	constexpr float MOUNTAIN_X      = 60.0f;
	constexpr float MOUNTAIN_Z      = 80.0f;
	constexpr float MOUNTAIN_RADIUS = 20.0f;
	constexpr float MOUNTAIN_HEIGHT = 9.0f;

	// 川の谷
	constexpr float RIVER_EXTRA_BEND_START_Z = 25.0f;
	constexpr float RIVER_EXTRA_BEND_END_Z   = 55.0f;
	constexpr float RIVER_EXTRA_BEND_AMOUNT  = 12.0f;

	constexpr float RIVER_BANK_WIDTH    = 16.0f;
	constexpr float RIVER_TERRAIN_DEPTH = 1.2f;

	// 道のカーブ
	constexpr float PATH_CURVE_FREQUENCY_A = 3.0f;
	constexpr float PATH_CURVE_AMPLITUDE_A = 18.0f;
	constexpr float PATH_CURVE_FREQUENCY_B = 1.5f;
	constexpr float PATH_CURVE_AMPLITUDE_B = 10.0f;

	// 採取物の配置禁止範囲
	constexpr float SPIRAL_HILL_MARGIN = 12.0f;
	constexpr float START_CLEARANCE    = 4.0f;
	constexpr float GOAL_CLEARANCE     = 5.0f;

	// 法線計算などで使用する許容誤差
	constexpr float EPSILON = 0.0001f;
}  // namespace MapConfig

// ============================================================
// 内部補助関数
// このファイル内だけで使用する共通処理です。
// ============================================================

namespace
{
	constexpr float PI = 3.14159265358979323846f;

	// 値を指定範囲内に収める
	float ClampValue(
	    float value,
	    float minValue,
	    float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	// 2つの値を線形補間する
	float Lerp(
	    float start,
	    float end,
	    float rate)
	{
		return start + (end - start) * rate;
	}

	// 0～1の範囲で滑らかに変化する補間値を求める
	float SmoothStep(
	    float rate)
	{
		rate = ClampValue(rate, 0.0f, 1.0f);
		return rate * rate * (3.0f - 2.0f * rate);
	}

	// XZ平面上の2点間の距離を求める
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

	// --------------------------------------------------------
	// 川の中心座標
	// IsRiver() が使用する川の中心線です。
	// --------------------------------------------------------
	float GetRiverCenterX(
	    float z)
	{
		return MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A +
		       std::sin(z * MapConfig::RIVER_FREQUENCY_B + MapConfig::RIVER_PHASE_B) * MapConfig::RIVER_BEND_B;
	}

	// --------------------------------------------------------
	// 地形生成用の川の中心座標
	// 地形に谷を作るための追加カーブを含みます。
	// --------------------------------------------------------
	float GetTerrainRiverCenterX(
	    float z)
	{
		float centerX =
		    MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A;

		if (z >= MapConfig::RIVER_EXTRA_BEND_START_Z && z <= MapConfig::RIVER_EXTRA_BEND_END_Z)
		{
			const float RANGE = MapConfig::RIVER_EXTRA_BEND_END_Z - MapConfig::RIVER_EXTRA_BEND_START_Z;

			const float RATE = (z - MapConfig::RIVER_EXTRA_BEND_START_Z) / RANGE;

			centerX -= std::sin(RATE * PI) * MapConfig::RIVER_EXTRA_BEND_AMOUNT;
		}

		return centerX;
	}

	// --------------------------------------------------------
	// 渦巻き丘の配置禁止判定
	// 丘の内部と、その周囲の余白を配置禁止範囲にします。
	// --------------------------------------------------------
	bool IsInsideSpiralHill(
	    float x,
	    float z)
	{
		const float DISTANCE = GetDistance(x, z, MapConfig::SPIRAL_HILL_X, MapConfig::SPIRAL_HILL_Z);

		const float EXCLUSION_RADIUS = MapConfig::SPIRAL_HILL_RADIUS + MapConfig::SPIRAL_HILL_MARGIN;

		return DISTANCE <= EXCLUSION_RADIUS;
	}

	// --------------------------------------------------------
	// 渦巻き丘の入口を地形に反映する
	// --------------------------------------------------------
	float ApplySpiralEntrance(
	    float height,
	    float x,
	    float z)
	{
		const float CENTER_X = MapConfig::SPIRAL_HILL_X;
		const float START_Z  = MapConfig::SPIRAL_HILL_Z + MapConfig::SPIRAL_HILL_RADIUS;

		const float END_Z = START_Z - MapConfig::ENTRANCE_LENGTH;

		const float DISTANCE_X = std::abs(x - CENTER_X);

		const bool IS_IN_ENTRANCE = DISTANCE_X <= MapConfig::ENTRANCE_WIDTH * 0.5f && z <= START_Z && z >= END_Z;

		if (!IS_IN_ENTRANCE)
		{
			return height;
		}

		// 入口の開始地点から奥に進むほど高さを上げる
		const float ENTRANCE_RATE = ClampValue((START_Z - z) / MapConfig::ENTRANCE_LENGTH, 0.0f, 1.0f);

		const float TARGET_HEIGHT = MapConfig::BASE_HEIGHT + ENTRANCE_RATE * MapConfig::ENTRANCE_HEIGHT;

		// 入口の中央ほど強く地形を下げる
		const float BLEND_RATE =
		    SmoothStep(ClampValue(1.0f - DISTANCE_X / (MapConfig::ENTRANCE_WIDTH * 0.5f), 0.0f, 1.0f));

		return Lerp(height, std::min(height, TARGET_HEIGHT), BLEND_RATE);
	}

	// --------------------------------------------------------
	// 渦巻き丘の内部に床を作る
	// --------------------------------------------------------
	float ApplyBackFloor(
	    float height,
	    float x,
	    float z)
	{
		const float DISTANCE_X = std::abs(x - MapConfig::SPIRAL_HILL_X);

		const float DISTANCE_Z = std::abs(z - MapConfig::SPIRAL_HILL_Z);

		const float BLEND_DISTANCE = MapConfig::BACK_FLOOR_BLEND;

		const float RATE_X =
		    ClampValue((MapConfig::BACK_FLOOR_WIDTH * 0.5f + BLEND_DISTANCE - DISTANCE_X) / BLEND_DISTANCE, 0.0f, 1.0f);

		const float RATE_Z = ClampValue(
		    (MapConfig::BACK_FLOOR_LENGTH * 0.5f + BLEND_DISTANCE - DISTANCE_Z) / BLEND_DISTANCE,
		    0.0f,
		    1.0f);

		const float BLEND_RATE = SmoothStep(std::min(RATE_X, RATE_Z));

		const float TARGET_HEIGHT = MapConfig::BASE_HEIGHT + MapConfig::BACK_FLOOR_HEIGHT;

		return Lerp(height, TARGET_HEIGHT, BLEND_RATE);
	}

	// --------------------------------------------------------
	// 段丘の高さを追加する
	// --------------------------------------------------------
	float AddTerrace(
	    float height,
	    float x,
	    float z,
	    float centerX,
	    float centerZ,
	    float radius,
	    float terraceHeight)
	{
		const float DISTANCE = GetDistance(x, z, centerX, centerZ);

		if (DISTANCE >= radius)
		{
			return height;
		}

		const float RATE = DISTANCE / radius;

		return height + (1.0f - SmoothStep(RATE)) * terraceHeight;
	}

	// --------------------------------------------------------
	// 山の高さを追加する
	// --------------------------------------------------------
	float AddMountain(
	    float height,
	    float x,
	    float z)
	{
		const float DISTANCE = GetDistance(x, z, MapConfig::MOUNTAIN_X, MapConfig::MOUNTAIN_Z);

		if (DISTANCE >= MapConfig::MOUNTAIN_RADIUS)
		{
			return height;
		}

		const float RATE = DISTANCE / MapConfig::MOUNTAIN_RADIUS;

		return height + (1.0f - SmoothStep(RATE)) * MapConfig::MOUNTAIN_HEIGHT;
	}

	// --------------------------------------------------------
	// 川の谷を地形に反映する
	// --------------------------------------------------------
	float ApplyRiverTerrain(
	    float height,
	    float x,
	    float z)
	{
		const float CENTER_X = GetTerrainRiverCenterX(z);
		const float DISTANCE = std::abs(x - CENTER_X);

		const float RATE = ClampValue(DISTANCE / MapConfig::RIVER_BANK_WIDTH, 0.0f, 1.0f);

		const float DEPRESSION = (1.0f - SmoothStep(RATE)) * MapConfig::RIVER_TERRAIN_DEPTH;

		return height - DEPRESSION;
	}
}  // namespace

// ============================================================
// MapData：初期化
// ============================================================

MapData::MapData(
    const GameObject _objHarvestableController) :
    objHarvestableController{ _objHarvestableController }
{
	Generate();
}

// ============================================================
// MapData：配列の添字を計算する
// ============================================================

int32_t MapData::GetIndex(
    int32_t x,
    int32_t z) const
{
	return z * (MapConfig::MAP_WIDTH + 1) + x;
}

// ============================================================
// MapData：マップの範囲内か判定する
// ============================================================

bool MapData::IsInsideMap(
    float x,
    float z) const
{
	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	return x >= -HALF_WIDTH && x < HALF_WIDTH && z >= -HALF_DEPTH && z < HALF_DEPTH;
}

// ============================================================
// MapData：地形の高さを計算する
// 各地形の処理を順番に適用します。
// ============================================================

float MapData::CalculateRawHeight(
    float x,
    float z) const
{
	// 1. 地形全体に細かな起伏を追加する
	float height = MapConfig::BASE_HEIGHT +
	               std::sin(x * MapConfig::TERRAIN_WAVE_X_FREQUENCY) * MapConfig::TERRAIN_WAVE_HEIGHT +
	               std::cos(z * MapConfig::TERRAIN_WAVE_Z_FREQUENCY) * MapConfig::TERRAIN_WAVE_HEIGHT;

	// 2. 北東方向に向かって徐々に高くする
	const float SLOPE_RATE = ClampValue((x + z + MapConfig::SLOPE_OFFSET) / MapConfig::SLOPE_DISTANCE, 0.0f, 1.0f);

	height += SmoothStep(SLOPE_RATE) * MapConfig::NORTH_EAST_RISE;

	// 3. 渦巻き丘を生成する
	const float DX = x - MapConfig::SPIRAL_HILL_X;
	const float DZ = z - MapConfig::SPIRAL_HILL_Z;

	const float DISTANCE = std::sqrt(DX * DX + DZ * DZ);
	const float RADIUS   = MapConfig::SPIRAL_HILL_RADIUS;

	if (DISTANCE < RADIUS)
	{
		const float RATE  = DISTANCE / RADIUS;
		const float ANGLE = std::atan2(DZ, DX);

		// 外周を高くする
		const float OUTER_HEIGHT = SmoothStep(RATE) * MapConfig::SPIRAL_HILL_HEIGHT;

		// 渦巻き状の尾根を作る
		const float SPIRAL_ANGLE = ANGLE + RATE * MapConfig::SPIRAL_TURNS * 2.0f * PI;

		const float RIDGE = std::cos(SPIRAL_ANGLE) * MapConfig::SPIRAL_RIDGE_HEIGHT * SmoothStep(RATE);

		// 中央をくぼませる
		const float CENTER_BASIN = (1.0f - SmoothStep(RATE)) * MapConfig::SPIRAL_CENTER_BASIN_DEPTH;

		height += OUTER_HEIGHT + RIDGE - CENTER_BASIN;

		// 3-1. 丘の入口を作る
		height = ApplySpiralEntrance(height, x, z);

		// 3-2. 丘の内部に床を作る
		height = ApplyBackFloor(height, x, z);
	}

	// 4. 左側の段丘を作る
	height = AddTerrace(
	    height,
	    x,
	    z,
	    MapConfig::LEFT_TERRACE_X,
	    MapConfig::LEFT_TERRACE_Z,
	    MapConfig::LEFT_TERRACE_RADIUS,
	    MapConfig::LEFT_TERRACE_HEIGHT);

	// 5. 右側の段丘を作る
	height = AddTerrace(
	    height,
	    x,
	    z,
	    MapConfig::RIGHT_TERRACE_X,
	    MapConfig::RIGHT_TERRACE_Z,
	    MapConfig::RIGHT_TERRACE_RADIUS,
	    MapConfig::RIGHT_TERRACE_HEIGHT);

	// 6. 北東の山を作る
	height = AddMountain(height, x, z);

	// 7. 川に沿って谷を作る
	height = ApplyRiverTerrain(height, x, z);

	// 8. 高さをマップの設定範囲内に収める
	return ClampValue(height, MapConfig::BASE_HEIGHT, MapConfig::MAX_HEIGHT);
}

// ============================================================
// MapData：地形データを生成する
// ============================================================

void MapData::Generate()
{
	const int32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;
	const int32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	heights.resize(static_cast<size_t>(VERTEX_WIDTH) * static_cast<size_t>(VERTEX_DEPTH));

	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			heights[GetIndex(x, z)] = CalculateRawHeight(WORLD_X, WORLD_Z);
		}
	}
}

// ============================================================
// MapData：指定位置の地形の高さを取得する
// ============================================================



float MapData::GetHeight(float x,float z) const
{
	// =========================================================
	// 1. 地形データの確認
	// =========================================================

	if (heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	// =========================================================
	// 2. 座標変換に使用する定数
	// =========================================================

	// マップの中心から端までの距離を求める係数
	constexpr float HALF_SCALE = 0.5f;

	// グリッド内の相対位置の範囲
	constexpr float GRID_RATIO_MIN = 0.0f;
	constexpr float GRID_RATIO_MAX = 1.0f;

	// マップの中心を基準に、ワールド座標をグリッド座標へ変換
	const float HALF_WIDTH = static_cast<float>(MapConfig::MAP_WIDTH) * MapConfig::TILE_SIZE * HALF_SCALE;

	const float HALF_DEPTH = static_cast<float>(MapConfig::MAP_DEPTH) * MapConfig::TILE_SIZE * HALF_SCALE;

	const float GRID_X =
	    ClampValue((x + HALF_WIDTH) / MapConfig::TILE_SIZE, GRID_RATIO_MIN, static_cast<float>(MapConfig::MAP_WIDTH));

	const float GRID_Z =
	    ClampValue((z + HALF_DEPTH) / MapConfig::TILE_SIZE, GRID_RATIO_MIN, static_cast<float>(MapConfig::MAP_DEPTH));

	// =========================================================
	// 3. 周囲の頂点を取得
	// =========================================================

	const int32_t X0 = static_cast<int32_t>(std::floor(GRID_X));
	const int32_t Z0 = static_cast<int32_t>(std::floor(GRID_Z));

	// マップの範囲外の頂点を参照しないようにする
	const int32_t X1 = std::min(X0 + 1, MapConfig::MAP_WIDTH);

	const int32_t Z1 = std::min(Z0 + 1, MapConfig::MAP_DEPTH);

	// グリッド内での相対位置を求める
	const float FX = GRID_X - static_cast<float>(X0);
	const float FZ = GRID_Z - static_cast<float>(Z0);

	// 周囲4頂点の高さを取得
	const float H00 = heights[GetIndex(X0, Z0)];
	const float H10 = heights[GetIndex(X1, Z0)];
	const float H01 = heights[GetIndex(X0, Z1)];
	const float H11 = heights[GetIndex(X1, Z1)];

	// =========================================================
	// 4. 三角形の分割位置を判定
	// =========================================================

	// グリッド内の対角線上にあるかを判定する
	constexpr float TRIANGLE_SPLIT_RATIO = 1.0f;

	if (FX + FZ <= TRIANGLE_SPLIT_RATIO)
	{
		// 三角形1：H00、H10、H01
		return H00 + FX * (H10 - H00) + FZ * (H01 - H00);
	}

	// 三角形2：H10、H11、H01
	return H10 * (GRID_RATIO_MAX - FZ) + H11 * (FX + FZ - TRIANGLE_SPLIT_RATIO) + H01 * (GRID_RATIO_MAX - FX);
}



// ============================================================
// MapData：指定位置が道か判定する
// ============================================================

bool MapData::IsPath(
    float x,
    float z) const
{
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	const float MAP_LENGTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE;

	const float RATE = ClampValue((z + HALF_DEPTH) / MAP_LENGTH, 0.0f, 1.0f);

	const float PATH_CENTER_X =
	    Lerp(MapConfig::PATH_START_X, MapConfig::PATH_GOAL_X, RATE) +
	    std::sin(RATE * MapConfig::PATH_CURVE_FREQUENCY_A * PI) * MapConfig::PATH_CURVE_AMPLITUDE_A +
	    std::cos(RATE * MapConfig::PATH_CURVE_FREQUENCY_B * PI) * MapConfig::PATH_CURVE_AMPLITUDE_B;

	return std::abs(x - PATH_CENTER_X) <= MapConfig::PATH_WIDTH * 0.5f;
}

// ============================================================
// MapData：指定位置が川か判定する
// ============================================================

bool MapData::IsRiver(
    float x,
    float z) const
{
	return std::abs(x - GetRiverCenterX(z)) <= MapConfig::RIVER_WIDTH * 0.5f;
}

// ============================================================
// MapData：スタート位置を取得する
// ============================================================

Vec3 MapData::GetStartPosition() const
{
	const float X = MapConfig::PATH_START_X;
	const float Z = MapConfig::PATH_START_Z;

	return Vec3{ X, GetHeight(X, Z), Z };
}

// ============================================================
// MapData：ゴール位置を取得する
// ============================================================

Vec3 MapData::GetGoalPosition() const
{
	const float X = MapConfig::PATH_GOAL_X;
	const float Z = MapConfig::PATH_GOAL_Z;

	return Vec3{ X, GetHeight(X, Z), Z };
}

// 既存コードとの互換性を維持するための関数
Vec3 MapData::GetBeginPosision() const
{
	return GetStartPosition();
}

Vec3 MapData::GetEndPosision() const
{
	return GetGoalPosition();
}

// ============================================================
// MAP：初期化
// ============================================================

void MAP::Start()
{
	// 地形を地形管理システムに登録する
	Get<TerrainSystem>().Register(this->ToGameObject());

	// 地形マテリアルを作成する
	MaterialResource materialResource{};

	std::unique_ptr<TerrainMaterialProperty> property = std::make_unique<TerrainMaterialProperty>();

	property->Get().hasTexture = { TRUE, TRUE, TRUE, TRUE };
	property->Get().baseColor  = { 1, 1, 1, 1 };

	materialResource.TryLoad(std::move(property));
	material = Resource<Material>{ std::move(materialResource) };

	// 描画用メッシュを生成する
	Reload();

	// --------------------------------------------------------
	// 木・岩の配置データ
	// Y座標は配置時に地形の高さから取得します。
	// --------------------------------------------------------

	struct HarvestablePlacement
	{
		HarvestableType type;
		float x;
		float z;
		float scale;
	};
	
	struct HarvestableEntity
	{
		ItemId itemId;
		float x;
		float z;
	};
	
	const HarvestableEntity harvestableEntities[] = {
		// ==============================
		// 木のアイテム（16個）
		// ==============================
		{ 8, -86.0f, -91.0f },
		{ 8, -82.0f, -98.0f },
		{ 8, -94.0f, -86.0f },
		{ 8, -78.0f, -91.0f },

		{ 8, -88.0f, -78.0f },
		{ 8, -73.0f, -96.0f },
		{ 8, -68.0f, -88.0f },
		{ 8, -99.0f, -91.0f },

		{ 8, -62.0f, -96.0f },
		{ 8, -76.0f, -72.0f },
		{ 8, -58.0f, -82.0f },
		{ 8, -91.0f, -60.0f },

		{ 8, -52.0f, -95.0f },
		{ 8, -65.0f, -65.0f },
		{ 8, -45.0f, -78.0f },
		{ 8, -55.0f, -55.0f },

		// ==============================
		// 石のアイテム（16個）
		// ==============================
		{ 7, -84.0f, -84.0f },
		{ 7, -76.0f, -97.0f },
		{ 7, -97.0f, -99.0f },
		{ 7, -70.0f, -85.0f },

		{ 7, -82.0f, -68.0f },
		{ 7, -62.0f, -88.0f },
		{ 7, -56.0f, -92.0f },
		{ 7, -96.0f, -55.0f },

		{ 7, -48.0f, -96.0f },
		{ 7, -68.0f, -55.0f },
		{ 7, -42.0f, -88.0f },
		{ 7, -88.0f, -48.0f },

		{ 7, -55.0f, -42.0f },
		{ 7, -38.0f, -70.0f },
		{ 7, -72.0f, -40.0f },
		{ 7, -45.0f, -50.0f },

		
	};
	for (const auto& entity : harvestableEntities)
	{
		const float y = GetHeight(entity.x, entity.z);
		Get<EntitySystem>().Spawn(entity.itemId, Vec3(entity.x,y,entity.z), Vec3());
	}

	const HarvestablePlacement placements[] = {
		// スタート付近の木
		{ HarvestableType::Tree, -96.0f, -94.0f, 5.0f },
		{ HarvestableType::Tree, -92.0f, -86.0f, 5.0f },
		{ HarvestableType::Tree, -84.0f, -96.0f, 5.0f },
		{ HarvestableType::Tree, -80.0f, -89.0f, 5.0f },
		{ HarvestableType::Tree, -95.0f, -80.0f, 5.0f },
		{ HarvestableType::Tree, -76.0f, -82.0f, 5.0f },
		{ HarvestableType::Tree, -70.0f, -94.0f, 5.0f },
		{ HarvestableType::Tree, -65.0f, -85.0f, 5.0f },
		{ HarvestableType::Tree, -98.0f, -68.0f, 5.0f },
		{ HarvestableType::Tree, -86.0f, -64.0f, 5.0f },
		{ HarvestableType::Tree, -72.0f, -70.0f, 5.0f },

		{ HarvestableType::TreeHigh, -94.0f, -88.0f, 5.2f },
		{ HarvestableType::TreeHigh, -78.0f, -96.0f, 5.2f },
		{ HarvestableType::TreeHigh, -98.0f, -78.0f, 5.2f },
		{ HarvestableType::TreeHigh, -68.0f, -76.0f, 5.2f },

		// マップ各地の木
		{ HarvestableType::Tree, -85.0f, -65.0f, 5.0f },
		{ HarvestableType::Tree, -30.0f, -45.0f, 5.0f },
		{ HarvestableType::Tree, 10.0f, -60.0f, 5.0f },
		{ HarvestableType::Tree, 30.0f, -40.0f, 5.0f },
		{ HarvestableType::Tree, -80.0f, -20.0f, 5.0f },
		{ HarvestableType::Tree, -55.0f, 0.0f, 5.0f },
		{ HarvestableType::Tree, 15.0f, 10.0f, 5.0f },
		{ HarvestableType::Tree, 45.0f, 30.0f, 5.0f },
		{ HarvestableType::Tree, -35.0f, -85.0f, 5.0f },
		{ HarvestableType::Tree, 5.0f, -85.0f, 5.0f },
		{ HarvestableType::Tree, 25.0f, -90.0f, 5.0f },
		{ HarvestableType::Tree, 45.0f, -80.0f, 5.0f },
		{ HarvestableType::Tree, 65.0f, -85.0f, 5.0f },
		{ HarvestableType::Tree, 85.0f, -75.0f, 5.0f },
		{ HarvestableType::Tree, -90.0f, -50.0f, 5.0f },
		{ HarvestableType::Tree, 5.0f, -20.0f, 5.0f },
		{ HarvestableType::Tree, 25.0f, -15.0f, 5.0f },
		{ HarvestableType::Tree, 50.0f, -25.0f, 5.0f },
		{ HarvestableType::Tree, 75.0f, -30.0f, 5.0f },
		{ HarvestableType::Tree, 90.0f, -10.0f, 5.0f },
		{ HarvestableType::Tree, -90.0f, 5.0f, 5.0f },
		{ HarvestableType::Tree, -5.0f, 35.0f, 5.0f },
		{ HarvestableType::Tree, 20.0f, 30.0f, 5.0f },
		{ HarvestableType::Tree, 40.0f, 45.0f, 5.0f },
		{ HarvestableType::Tree, 65.0f, 35.0f, 5.0f },
		{ HarvestableType::Tree, 85.0f, 25.0f, 5.0f },
		{ HarvestableType::Tree, -80.0f, 45.0f, 5.0f },
		{ HarvestableType::Tree, -35.0f, 50.0f, 5.0f },
		{ HarvestableType::Tree, -10.0f, 60.0f, 5.0f },
		{ HarvestableType::Tree, 15.0f, 55.0f, 5.0f },
		{ HarvestableType::Tree, 35.0f, 65.0f, 5.0f },
		{ HarvestableType::Tree, 80.0f, 70.0f, 5.0f },
		{ HarvestableType::Tree, -90.0f, 80.0f, 5.0f },
		{ HarvestableType::Tree, 10.0f, 80.0f, 5.0f },
		{ HarvestableType::Tree, 35.0f, 90.0f, 5.0f },
		{ HarvestableType::Tree, 85.0f, 90.0f, 5.0f },

		// 背の高い木
		{ HarvestableType::TreeHigh, -75.0f, -40.0f, 5.2f },
		{ HarvestableType::TreeHigh, -35.0f, -65.0f, 5.2f },
		{ HarvestableType::TreeHigh, 25.0f, -20.0f, 5.2f },
		{ HarvestableType::TreeHigh, -75.0f, 15.0f, 5.2f },
		{ HarvestableType::TreeHigh, -5.0f, -70.0f, 5.2f },
		{ HarvestableType::TreeHigh, 55.0f, -75.0f, 5.2f },
		{ HarvestableType::TreeHigh, 80.0f, -55.0f, 5.2f },
		{ HarvestableType::TreeHigh, -85.0f, 40.0f, 5.2f },
		{ HarvestableType::TreeHigh, 25.0f, 50.0f, 5.2f },
		{ HarvestableType::TreeHigh, 60.0f, 45.0f, 5.2f },
		{ HarvestableType::TreeHigh, 85.0f, 60.0f, 5.2f },
		{ HarvestableType::TreeHigh, 15.0f, 75.0f, 5.2f },
		{ HarvestableType::TreeHigh, 50.0f, 85.0f, 5.2f },

		// 道中の岩
		{ HarvestableType::Rock, -72.0f, -55.0f, 3.0f },
		{ HarvestableType::Rock, -55.0f, -48.0f, 3.0f },
		{ HarvestableType::Rock, -38.0f, -35.0f, 3.0f },
		{ HarvestableType::Rock, -20.0f, -25.0f, 3.0f },
		{ HarvestableType::Rock, -5.0f, -10.0f, 3.0f },
		{ HarvestableType::Rock, 15.0f, -5.0f, 3.0f },
		{ HarvestableType::Rock, 35.0f, 5.0f, 3.0f },
		{ HarvestableType::Rock, -65.0f, -35.0f, 3.0f },
		{ HarvestableType::Rock, -48.0f, -20.0f, 3.0f },
		{ HarvestableType::Rock, -30.0f, -5.0f, 3.0f },
		{ HarvestableType::Rock, -10.0f, 10.0f, 3.0f },
		{ HarvestableType::Rock, 12.0f, 18.0f, 3.0f },
		{ HarvestableType::Rock, 32.0f, 30.0f, 3.0f },
		{ HarvestableType::Rock, 40.0f, 42.0f, 3.0f },
		{ HarvestableType::Rock, 48.0f, 50.0f, 3.0f },
		{ HarvestableType::Rock, 62.0f, 52.0f, 3.0f },
		{ HarvestableType::Rock, 72.0f, 62.0f, 3.0f },
		{ HarvestableType::Rock, 43.0f, 78.0f, 3.0f },
		{ HarvestableType::Rock, 68.0f, 82.0f, 3.0f },

		// マップの端側の岩
		{ HarvestableType::Rock, -90.0f, -45.0f, 3.0f },
		{ HarvestableType::Rock, -30.0f, -80.0f, 3.0f },
		{ HarvestableType::Rock, 0.0f, -35.0f, 3.0f },
		{ HarvestableType::Rock, 35.0f, -60.0f, 3.0f },
		{ HarvestableType::Rock, 55.0f, 15.0f, 3.0f },
		{ HarvestableType::Rock, 75.0f, 45.0f, 3.0f },
		{ HarvestableType::Rock, 10.0f, -90.0f, 3.0f },
		{ HarvestableType::Rock, 40.0f, -85.0f, 3.0f },
		{ HarvestableType::Rock, 70.0f, -90.0f, 3.0f },
		{ HarvestableType::Rock, 90.0f, -70.0f, 3.0f },
		{ HarvestableType::Rock, 5.0f, -45.0f, 3.0f },
		{ HarvestableType::Rock, 45.0f, -40.0f, 3.0f },
		{ HarvestableType::Rock, 80.0f, -35.0f, 3.0f },
		{ HarvestableType::Rock, -90.0f, -10.0f, 3.0f },
		{ HarvestableType::Rock, 5.0f, 15.0f, 3.0f },
		{ HarvestableType::Rock, 35.0f, 20.0f, 3.0f },
		{ HarvestableType::Rock, 70.0f, 5.0f, 3.0f },
		{ HarvestableType::Rock, 90.0f, 20.0f, 3.0f },
		{ HarvestableType::Rock, -85.0f, 30.0f, 3.0f },
		{ HarvestableType::Rock, -25.0f, 40.0f, 3.0f },
		{ HarvestableType::Rock, 5.0f, 50.0f, 3.0f },
		{ HarvestableType::Rock, 40.0f, 55.0f, 3.0f },
		{ HarvestableType::Rock, 75.0f, 65.0f, 3.0f },
		{ HarvestableType::Rock, 20.0f, 90.0f, 3.0f },
		{ HarvestableType::Rock, 50.0f, 80.0f, 3.0f },
		{ HarvestableType::Rock, 85.0f, 85.0f, 3.0f },
	};

	// 実際のスタート・ゴール座標
	const Vec3 START_POSITION{ -88.0f, 1.830f, -92.3f };
	const Vec3 GOAL_POSITION{ 53.21f, 16.0f, 68.91f };

	// 配置候補を1つずつ確認する
	for (const auto& placement : placements)
	{
		// マップの外には配置しない
		if (!IsInsideMap(placement.x, placement.z))
		{
			continue;
		}

		// 渦巻き丘の内部と周囲には配置しない
		if (IsInsideSpiralHill(placement.x, placement.z))
		{
			continue;
		}

		// スタート地点付近を空ける
		if (GetDistance(placement.x, placement.z, START_POSITION.x, START_POSITION.z) < MapConfig::START_CLEARANCE)
		{
			continue;
		}

		// ゴール地点付近を空ける
		if (GetDistance(placement.x, placement.z, GOAL_POSITION.x, GOAL_POSITION.z) < MapConfig::GOAL_CLEARANCE)
		{
			continue;
		}

		// 道の上には配置しない
		if (IsPath(placement.x, placement.z))
		{
			continue;
		}

		// 川の上には配置しない
		if (IsRiver(placement.x, placement.z))
		{
			continue;
		}

		// 地形の高さを取得して配置する
		const float GROUND_Y = GetHeight(placement.x, placement.z);

		const Vec3 POSITION{ placement.x, GROUND_Y, placement.z };

		Get<HarvestableController>(objHarvestableController).AddHarvestable(placement.type, POSITION, placement.scale);
	}
}

// ============================================================
// MAP：更新
// ============================================================

void MAP::Update() {}

// ============================================================
// MAP：地形を描画する
// ============================================================

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
// MAP：描画用メッシュを生成する
// ============================================================

void MAP::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& device)
	    {
		    using Vertex = ModelVertex;

		    const uint32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;

		    const uint32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

		    std::vector<Vertex> vertices(VERTEX_COUNT);

		    const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

		    const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

		    // ------------------------------------------------
		    // 1. 頂点の位置・UV・法線を計算する
		    // ------------------------------------------------
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

				    vertices[INDEX].uv = Vec2{ x * MapConfig::TILE_SIZE, z * MapConfig::TILE_SIZE };

				    // 周囲の高さから法線を計算する
				    const float LEFT_HEIGHT = GetHeight(position.x - MapConfig::TILE_SIZE, position.z);

				    const float RIGHT_HEIGHT = GetHeight(position.x + MapConfig::TILE_SIZE, position.z);

				    const float BACK_HEIGHT = GetHeight(position.x, position.z - MapConfig::TILE_SIZE);

				    const float FRONT_HEIGHT = GetHeight(position.x, position.z + MapConfig::TILE_SIZE);

				    const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT,
					                   2.0f * MapConfig::TILE_SIZE,
					                   BACK_HEIGHT - FRONT_HEIGHT };

				    const float NORMAL_LENGTH =
				        std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

				    if (NORMAL_LENGTH > MapConfig::EPSILON)
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
		    // 2. 三角形を構成するインデックスを作成する
		    // 1マスにつき三角形を2枚作成する
		    // ------------------------------------------------
		    const uint32_t INDEX_COUNT = MapConfig::MAP_WIDTH * MapConfig::MAP_DEPTH * 6;

		    std::vector<uint32_t> indices(INDEX_COUNT);

		    uint32_t index = 0;

		    for (uint32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
			    {
				    const uint32_t TOP_LEFT = z * VERTEX_WIDTH + x;

				    const uint32_t TOP_RIGHT = TOP_LEFT + 1;

				    const uint32_t BOTTOM_LEFT = (z + 1) * VERTEX_WIDTH + x;

				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    indices[index++] = TOP_LEFT;
				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_LEFT;

				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_RIGHT;
				    indices[index++] = BOTTOM_LEFT;
			    }
		    }

		    // ------------------------------------------------
		    // 3. 頂点バッファを作成する
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
		    // 4. インデックスバッファを作成する
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
		    // 5. 作成したバッファを地形サーフェスに登録する
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

// ============================================================
// MAP：指定位置の地形の高さを取得する
// ITerrainの実装
// ============================================================

float MAP::GetHeight(
    const float worldX,
    const float worldZ)
{
	if (heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	return MapData::GetHeight(worldX, worldZ);
}

// ============================================================
// MAP：指定位置の地形の法線を取得する
// ITerrainの実装
// ============================================================

Vec3 MAP::GetNormal(
    const float worldX,
    const float worldZ)
{
	if (heights.empty())
	{
		return Vec3::Up();
	}

	constexpr float SAMPLE_DISTANCE = MapConfig::TILE_SIZE;

	const float LEFT_HEIGHT = MapData::GetHeight(worldX - SAMPLE_DISTANCE, worldZ);

	const float RIGHT_HEIGHT = MapData::GetHeight(worldX + SAMPLE_DISTANCE, worldZ);

	const float BACK_HEIGHT = MapData::GetHeight(worldX, worldZ - SAMPLE_DISTANCE);

	const float FRONT_HEIGHT = MapData::GetHeight(worldX, worldZ + SAMPLE_DISTANCE);

	const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT, 2.0f * SAMPLE_DISTANCE, BACK_HEIGHT - FRONT_HEIGHT };

	const float NORMAL_LENGTH = std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

	if (NORMAL_LENGTH <= MapConfig::EPSILON)
	{
		return Vec3::Up();
	}

	return Vec3{ NORMAL.x / NORMAL_LENGTH, NORMAL.y / NORMAL_LENGTH, NORMAL.z / NORMAL_LENGTH };
}

Vec3 MAP::GetSize()
{
	return Vec3{ static_cast<float>(MapConfig::MAP_WIDTH), MapConfig::MAX_HEIGHT, static_cast<float>(MapConfig::MAP_DEPTH) };
}

Vec3 MAP::GetMinPosition()
{
	return Vec3();
}

Vec3 MAP::GetMaxPosition()
{
	return Vec3();
}
