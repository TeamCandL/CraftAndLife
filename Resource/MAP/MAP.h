
#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>
#include <Game/PlayScene/ITerrain.h>
#include <Game/PlayScene/ItemId.h>

// ============================================================
// マップ調整設定
// ここにある数値を変更することで、地形の形状を調整できます。
// ============================================================
namespace MapConfig
{
	// --------------------------------------------------------
	// 1. マップ全体の設定
	// --------------------------------------------------------

	constexpr int32_t MAP_WIDTH = 200;    // X方向のマス数
	constexpr int32_t MAP_DEPTH = 200;    // Z方向のマス数
	constexpr float TILE_SIZE   = 1.0f;   // 1マスのワールド上の大きさ
	constexpr float BASE_HEIGHT = 0.0f;   // 地形の基準となる高さ
	constexpr float MAX_HEIGHT  = 16.0f;  // 地形の最大高さ

	// --------------------------------------------------------
	// 2. ボクセルの段差設定
	// --------------------------------------------------------

	constexpr float VOXEL_STEP = 1.0f;  // 高さを段階的に変化させる単位

	// --------------------------------------------------------
	// 3. マップ全体の傾斜設定
	// 南西側から北東側へ向かう傾斜を調整します。
	// --------------------------------------------------------

	constexpr float SLOPE_OFFSET    = 100.0f;  // 傾斜を計算する際の位置補正
	constexpr float SLOPE_DISTANCE  = 180.0f;  // 傾斜が変化する距離
	constexpr float NORTH_EAST_RISE = 10.0f;   // 北東方向への高さの増加量

	// --------------------------------------------------------
	// 4. 渦巻き状の丘の設定
	// --------------------------------------------------------

	constexpr float SPIRAL_HILL_X      = -50.0f;  // 丘の中心X座標
	constexpr float SPIRAL_HILL_Z      = 45.0f;   // 丘の中心Z座標
	constexpr float SPIRAL_HILL_RADIUS = 40.0f;   // 丘の広がる範囲
	constexpr float SPIRAL_HILL_HEIGHT = 7.0f;    // 丘の高さ
	constexpr float SPIRAL_TURNS       = 2.5f;    // 渦巻きの回転数

	// --------------------------------------------------------
	// 5. スタートからゴールまでの道
	// --------------------------------------------------------

	constexpr float PATH_WIDTH   = 4.0f;    // 道の幅
	constexpr float PATH_START_X = -75.0f;  // スタート地点のX座標
	constexpr float PATH_START_Z = -80.0f;  // スタート地点のZ座標
	constexpr float PATH_GOAL_X  = 65.0f;   // ゴール地点のX座標
	constexpr float PATH_GOAL_Z  = 75.0f;   // ゴール地点のZ座標

	// --------------------------------------------------------
	// 6. 川の設定
	// --------------------------------------------------------

	constexpr float RIVER_WIDTH    = 5.0f;  // 川の幅
	constexpr float RIVER_CENTER_X = 0.0f;  // 川の基準となるX座標

	// 川の蛇行幅・周期
	constexpr float RIVER_FREQUENCY_A = 0.025f;  // 蛇行Aの周期
	constexpr float RIVER_BEND_A      = 22.0f;   // 蛇行Aの左右の振れ幅
	constexpr float RIVER_FREQUENCY_B = 0.060f;  // 蛇行Bの周期
	constexpr float RIVER_PHASE_B     = 1.5f;    // 蛇行Bの開始位置
	constexpr float RIVER_BEND_B      = 8.0f;    // 蛇行Bの左右の振れ幅

	// 川の周囲に谷を作るための設定
	constexpr float RIVER_VALLEY_WIDTH = 8.0f;  // 谷の幅
	constexpr float RIVER_VALLEY_DEPTH = 1.5f;  // 谷の深さ

	// --------------------------------------------------------
	// 7. 滝の設定
	// 現在、滝の描画に使用していない設定値です。
	// 滝を再実装する場合に使用します。
	// --------------------------------------------------------

	constexpr float WATERFALL_TOP_Z       = 65.0f;  // 滝の上端Z座標
	constexpr float WATERFALL_BOTTOM_Z    = 20.0f;  // 滝の下端Z座標
	constexpr int32_t WATERFALL_TIERS     = 4;      // 滝の段数
	constexpr float WATERFALL_STEP_HEIGHT = 2.0f;   // 1段あたりの高さ
	constexpr float WATER_SURFACE_OFFSET  = 0.15f;  // 水面の高さ補正

	// --------------------------------------------------------
	// 8. アイテム設置
	// --------------------------------------------------------
	const ItemId ITEM_ID_ROCK{ 7 };  ///< 石のアイテムId
	const ItemId ITEM_ID_TREE{ 8 };  ///< 木のアイテムId
}  // namespace MapConfig

// ============================================================
// マップデータ
// 地形の高さ計算、地形判定、座標計算を担当します。
// ============================================================
struct MapData
{
	MapData(const GameObject _objHarvestableController);

	// --------------------------------------------------------
	// 描画に使用するリソース
	// --------------------------------------------------------

	Resource<Shader> shader{ "Shader/Terrain.hlsl" };

	//地面テクスチャ変更場所
	Resource<Texture> texture{ "Texture/Terrain/TileGrass.png", true };
	//Resource<Texture> texture{ "Texture/Terrain/grass_01.png", true };

	Resource<Material> material{};
	Resource<Surface> surface{};

	// 川・滝用の描画リソース
	Resource<Material> waterMaterial{};
	Resource<Surface> waterSurface{};

	// マップのTransform
	Component<Transform> transform{};

	GameObject objHarvestableController{};  ///< ハーベスタブルオブジェクトの操作オブジェクト

	// --------------------------------------------------------
	// 地形データ
	// 頂点ごとの高さを保存します。
	// --------------------------------------------------------

	std::vector<float> heights{};

	// --------------------------------------------------------
	// 地形生成・座標計算
	// --------------------------------------------------------

	void Generate();                               // 地形データを生成する
	int32_t GetIndex(int32_t x, int32_t z) const;  // 配列の添字を計算する
	float GetHeight(float x, float z) const;       // 指定位置の地形の高さを取得する
	bool IsInsideMap(float x, float z) const;      // マップ内の座標か判定する

	// --------------------------------------------------------
	// 地形の範囲判定
	// --------------------------------------------------------

	bool IsPath(float x, float z) const;   // 指定位置が道か判定する
	bool IsRiver(float x, float z) const;  // 指定位置が川か判定する

	// --------------------------------------------------------
	// スタート・ゴール座標
	// --------------------------------------------------------

	Vec3 GetStartPosition() const;  // スタート地点を取得する
	Vec3 GetGoalPosition() const;   // ゴール地点を取得する

	// 既存コードとの互換性を維持するため、関数名を変更しない
	Vec3 GetBeginPosision() const;
	Vec3 GetEndPosision() const;

  private:
	// 指定位置の地形の高さを計算する内部処理
	float CalculateRawHeight(float x, float z) const;
};

// ============================================================
// マップ本体
// 地形データを使って初期化・更新・描画を行います。
// ITerrainを通して、ほかの処理から地形情報を利用できます。
// ============================================================
struct MAP : ObjectBase<MapData>, ITerrain
{
	using ObjectBase<MapData>::ObjectBase;

	// オブジェクトのライフサイクル
	void Start() override;   // 初期化
	void Update() override;  // 更新
	void Draw() override;    // 描画
	float GetHeight(const float _worldX, const float _worldZ) override;
  private:
	// 地形データから描画用メッシュを作り直す
	void Reload();

	// ITerrainの実装
	//float GetHeight(const float _worldX, const float _worldZ) override;
	Vec3 GetNormal(const float _worldX, const float _worldZ) override;

	/// @brief 地形のサイズを取得する
	/// @return 地形のサイズ
	Vec3 GetSize() override;

	/// @brief 最小地点のワールド座標を取得する
	/// @return 最小地点のワールド座標
	Vec3 GetMinPosition() override;

	/// @brief 最大地点のワールド座標を取得する
	/// @return 最大地点のワールド座標
	Vec3 GetMaxPosition() override;
};
