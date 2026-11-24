
#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>

// ============================================================
// メインマップの前方宣言
// ============================================================

class MAP;

// ============================================================
// 島の砂浜設定
// ============================================================

namespace IslandConfig
{
	// メインマップの大きさ
	constexpr float MAP_WIDTH = 200.0f;
	constexpr float MAP_DEPTH = 200.0f;

	// メインマップの半分
	constexpr float HALF_WIDTH = MAP_WIDTH * 0.5f;
	constexpr float HALF_DEPTH = MAP_DEPTH * 0.5f;

	// 砂浜の幅
	constexpr float BEACH_WIDTH = 8.0f;

	// 砂浜の外側に向かって下げる高さ
	constexpr float BEACH_DROP = 1.2f;

	// 砂浜の1マスの大きさ
	constexpr float TILE_SIZE = 1.0f;

	// 砂浜の頂点数
	constexpr int32_t VERTEX_WIDTH = static_cast<int32_t>((MAP_WIDTH + BEACH_WIDTH * 2.0f) / TILE_SIZE) + 1;

	constexpr int32_t VERTEX_DEPTH = static_cast<int32_t>((MAP_DEPTH + BEACH_WIDTH * 2.0f) / TILE_SIZE) + 1;

	// 内側の砂浜開始位置
	constexpr int32_t INNER_START = static_cast<int32_t>(BEACH_WIDTH / TILE_SIZE);

	// メインマップの頂点数
	constexpr int32_t MAIN_VERTEX_WIDTH = static_cast<int32_t>(MAP_WIDTH / TILE_SIZE) + 1;

	constexpr int32_t MAIN_VERTEX_DEPTH = static_cast<int32_t>(MAP_DEPTH / TILE_SIZE) + 1;

	// 法線計算の許容誤差
	constexpr float EPSILON = 0.0001f;
}  // namespace IslandConfig

// ============================================================
// 島の砂浜データ
// ============================================================

struct MAP_IslandData
{
	// メインマップのGameObject
	GameObject objMainMap{};

	// 砂浜専用のラスタライザー設定
	inline static const RasterizerConfig BEACH_RASTERIZER_CONFIG{
		D3D11_CULL_NONE,   // 表面・裏面の両方を描画
		D3D11_FILL_SOLID,  // 塗りつぶし
		true               // 反時計回りを表面と判定
	};

	// 砂浜専用シェーダー
	Resource<Shader> shader{ "Shader/Terrain.hlsl", BEACH_RASTERIZER_CONFIG } ;

	// 砂浜専用テクスチャ
	//Resource<Texture> texture{ "Texture/Terrain/Sand.png", true };
	Resource<Texture> texture{ "Texture/Terrain/TileSand.png", true };

	Resource<Material> material{};
	Resource<Surface> surface{};

	Component<Transform> transform{};

	// 砂浜の頂点データ
	std::vector<float> heights{};

	// メインマップを受け取る
	explicit MAP_IslandData(GameObject mainMap);

	// 地形生成
	void Generate(MAP& mainMap);

	// 配列の添字
	int32_t GetIndex(int32_t x, int32_t z) const;
};

// ============================================================
// 島の砂浜
// ============================================================

struct MAP_Island : ObjectBase<MAP_IslandData>
{
	using ObjectBase<MAP_IslandData>::ObjectBase;

	// 初期化
	void Start() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

  private:
	// 砂浜メッシュの生成
	void Reload();
};


