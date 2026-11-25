#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>
#include <Game/PlayScene/ITerrain.h>
#include <Game/PlayScene/ItemId.h>

// ============================================================
// SeaMAP 調整設定（島マップ・本島のみ / 川・滝なし）
// サイズ: 200x200 / 高さ: 0〜18 / 滑らかメッシュ
// ============================================================
namespace SeaMapConfig
{
	// --------------------------------------------------------
	// 1. マップ全体の設定
	// --------------------------------------------------------

	constexpr int32_t MAP_WIDTH = 200;    // X方向のマス数
	constexpr int32_t MAP_DEPTH = 200;    // Z方向のマス数
	constexpr float TILE_SIZE   = 1.0f;   // 1マスのワールド上の大きさ
	constexpr float BASE_HEIGHT = 0.0f;   // 地形の基準高さ
	constexpr float MAX_HEIGHT  = 18.0f;  // 地形の最大高さ（登れる高さに調整）
	constexpr float VOXEL_STEP  = 1.0f;   // ブロック段差の単位

	// --------------------------------------------------------
	// 2. 島の外形・海岸線（入り江・岬）の設定
	// --------------------------------------------------------

	constexpr float ISLAND_CENTER_X    = 0.0f;   // 島中心のX座標
	constexpr float ISLAND_CENTER_Z    = 0.0f;   // 島中心のZ座標
	constexpr float ISLAND_BASE_RADIUS = 78.0f;  // 島の基本半径
	constexpr float COAST_AMP_1        = 14.0f;  // 外周エッジの余白幅
	constexpr float COAST_AMP_2        = 5.5f;   // 海岸線の細かい凹凸幅
	constexpr float BEACH_HEIGHT       = 1.0f;   // 海岸・砂浜の高さ目安
	constexpr float LOWLAND_BASE       = 2.5f;   // 低地・森林エリアの基準高さ
	constexpr float TERRACE_STEP_SIZE  = 3.0f;   // 高台・段差のステップ幅

	// --------------------------------------------------------
	// 3. 中央山岳の設定（登りやすい傾斜・高さ）
	// --------------------------------------------------------

	constexpr float MOUNTAIN_CENTER_X = 0.0f;   // 山岳中心X座標
	constexpr float MOUNTAIN_CENTER_Z = 0.0f;   // 山岳中心Z座標
	constexpr float MOUNTAIN_RADIUS   = 72.0f;  // 裾野を広げて角度を緩やかにする
	constexpr float MOUNTAIN_PEAK     = 14.0f;  // 山頂付近の高さ目安

	// --------------------------------------------------------
	// 4. 基準地点・アイテム設定
	// --------------------------------------------------------

	constexpr float START_X = -55.0f;
	constexpr float START_Z = -55.0f;
	constexpr float GOAL_X  = 55.0f;
	constexpr float GOAL_Z  = 55.0f;

	const ItemId ITEM_ID_ROCK{ 7 };  ///< 石のアイテムId
	const ItemId ITEM_ID_TREE{ 8 };  ///< 木のアイテムId
}  // namespace SeaMapConfig

// ============================================================
// 地形種別（色分け・属性判定用）
// ============================================================
enum class SeaTerrainType : uint8_t
{
	Beach = 0,  // 海岸・砂浜
	Forest,     // 森林・低地
	Highland,   // 高台・段差
	Mountain,   // 中央山岳・山頂帯
};

// ============================================================
// SeaMapData
// 地形の高さ計算、地形判定、座標計算を担当します。
// ============================================================
struct SeaMapData
{
	SeaMapData(const GameObject _objHarvestableController);

	// --------------------------------------------------------
	// 描画に使用するリソース
	// --------------------------------------------------------

	Resource<Shader> shader{ "Shader/Terrain.hlsl" };
	Resource<Shader> shaderShadowMap{ "Shader/ShadowMap.hlsl" };

	Resource<Texture> texture{ "Texture/Terrain/TileGrass.png", true };

	Resource<Material> material{};
	Resource<Surface> surface{};

	// マップのTransform
	Component<Transform> transform{};

	GameObject objHarvestableController{};  ///< ハーベスタブルオブジェクトの操作オブジェクト

	// --------------------------------------------------------
	// 地形データ
	// --------------------------------------------------------

	std::vector<float> heights{};                // 頂点ごとの高さ
	std::vector<SeaTerrainType> terrainTypes{};  // 各セルの地形種別

	// --------------------------------------------------------
	// 地形生成・座標計算
	// --------------------------------------------------------

	void Generate();                               // 地形データを生成する
	int32_t GetIndex(int32_t x, int32_t z) const;  // 配列の添字を計算する
	float GetHeight(float x, float z) const;       // 指定位置の地形の高さを取得する
	SeaTerrainType GetTerrainType(float x, float z) const;
	bool IsInsideMap(float x, float z) const;  // マップ内の座標か判定する

	// 互換性維持用の範囲判定（川・道なし）
	bool IsPath(float x, float z) const;
	bool IsRiver(float x, float z) const;

	// --------------------------------------------------------
	// スタート・ゴール座標
	// --------------------------------------------------------

	Vec3 GetStartPosition() const;
	Vec3 GetGoalPosition() const;

	Vec3 GetBeginPosision() const;
	Vec3 GetEndPosision() const;

  private:
	float CalculateRawHeight(float x, float z) const;
	SeaTerrainType EvaluateTerrainType(float height) const;
};

// ============================================================
// SEAMAP
// 地形データを使って初期化・更新・描画を行います。
// ============================================================
struct SEAMAP : ObjectBase<SeaMapData>, ITerrain
{
	using ObjectBase<SeaMapData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;
	float GetHeight(const float _worldX, const float _worldZ) override;

  private:
	void Reload();

	// ITerrainの実装
	Vec3 GetNormal(const float _worldX, const float _worldZ) override;
	Vec3 GetSize() override;
	Vec3 GetMinPosition() override;
	Vec3 GetMaxPosition() override;

	void LoadMapPlacementCSV();
	void PlaceMapObject(int32_t type, float x, float z);
};