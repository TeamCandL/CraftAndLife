#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>
#include <Game/PlayScene/ITerrain.h>

/// @brief マップの調整値（設計図「滝と川のボクセル地形マップ」完全準拠）
namespace MapConfig
{
	// ========================================================
	// 1. マップ全体の大きさ
	// ========================================================
	constexpr int32_t MAP_WIDTH = 200;  // X方向のマス数
	constexpr int32_t MAP_DEPTH = 200;  // Z方向のマス数
	constexpr float TILE_SIZE = 1.0f;   // 1マスの大きさ
	constexpr float BASE_HEIGHT = 0.0f; // 地形の最低基準
	constexpr float MAX_HEIGHT = 16.0f; // 地形の最高高さ（北東の山頂）

	// ========================================================
	// 2. ボクセル段差（ステップ）設定
	// ========================================================
	constexpr float VOXEL_STEP = 1.0f; // ボクセルの1段の高さ

	// ========================================================
	// 3. 北東へ向かう全体的な傾斜
	// ========================================================
	constexpr float SLOPE_OFFSET = 100.0f;
	constexpr float SLOPE_DISTANCE = 180.0f;
	constexpr float NORTH_EAST_RISE = 10.0f;

	// ========================================================
	// 4. 左上の渦巻き丘 (Spiral Hill)
	// ========================================================
	constexpr float SPIRAL_HILL_X = -50.0f;
	constexpr float SPIRAL_HILL_Z = 45.0f;
	constexpr float SPIRAL_HILL_RADIUS = 40.0f;
	constexpr float SPIRAL_HILL_HEIGHT = 7.0f;
	constexpr float SPIRAL_TURNS = 2.5f; // 渦の回転数

	// ========================================================
	// 5. 道（S字カーブ：南西スタート -> 北東ゴール）
	// ========================================================
	constexpr float PATH_WIDTH = 4.0f;
	constexpr float PATH_START_X = -75.0f;
	constexpr float PATH_START_Z = -80.0f; // 青ピン位置
	constexpr float PATH_GOAL_X = 65.0f;
	constexpr float PATH_GOAL_Z = 75.0f; // 黄ピン位置

	// ========================================================
	// 6. 川と階段状の滝 (River & Waterfall)
	// ========================================================
	// 川の幅
	constexpr float RIVER_WIDTH = 5.0f;

	// 川の中心
	constexpr float RIVER_CENTER_X = 0.0f;

	// 川の蛇行
	constexpr float RIVER_FREQUENCY_A = 0.025f;
	constexpr float RIVER_BEND_A      = 22.0f;
	constexpr float RIVER_FREQUENCY_B = 0.060f;
	constexpr float RIVER_PHASE_B     = 1.5f;
	constexpr float RIVER_BEND_B      = 8.0f;

	// 川岸の谷
	constexpr float RIVER_VALLEY_WIDTH = 8.0f;
	constexpr float RIVER_VALLEY_DEPTH = 1.5f;

	// 滝の設定は使わない
	constexpr float WATERFALL_TOP_Z       = 65.0f;
	constexpr float WATERFALL_BOTTOM_Z    = 20.0f;
	constexpr int32_t WATERFALL_TIERS     = 4;
	constexpr float WATERFALL_STEP_HEIGHT = 2.0f;
	constexpr float WATER_SURFACE_OFFSET  = 0.15f;

} // namespace MapConfig

/// @brief マップデータ
struct MapData
{
	MapData();

	// 描画リソース
	Resource<Shader> shader{"Shader/Terrain.hlsl"};
	Resource<Texture> texture{"Texture/Terrain/TileGrass.png", true};

	Resource<Material> material{};
	Resource<Surface> surface{};

	// 水（川・滝）用
	Resource<Material> waterMaterial{};
	Resource<Surface> waterSurface{};

	Component<Transform> transform{};

	// 各頂点およびセルの高さデータ
	std::vector<float> heights{};

	// 生成・演算関数
	void Generate();

	int32_t GetIndex(int32_t x, int32_t z) const;
	float GetHeight(float x, float z) const;
	bool IsInsideMap(float x, float z) const;

	bool IsPath(float x, float z) const;
	bool IsRiver(float x, float z) const;

	Vec3 GetStartPosition() const;
	Vec3 GetGoalPosition() const;

	Vec3 GetBeginPosision() const;
	Vec3 GetEndPosision() const;



private:
	float CalculateRawHeight(float x, float z) const;
};

/// @brief マップオブジェクト
struct MAP : ObjectBase<MapData>,  ITerrain
{
	using ObjectBase<MapData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

private:
	void Reload();

  // ITerrain を介して継承されました
  float GetHeight(const float _worldX, const float _worldZ) override;
	Vec3 GetNormal(const float _worldX, const float _worldZ) override;
};