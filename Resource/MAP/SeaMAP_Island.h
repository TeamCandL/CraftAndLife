
#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>

// ============================================================
// メインマップの前方宣言
// ============================================================

struct SEAMAP;

// ============================================================
// 島の砂浜設定
// ============================================================

namespace IslandConfig
{
	// メインマップの大きさ
	constexpr float SEAMAP_WIDTH = 200.0f;
	constexpr float SEAMAP_DEPTH = 200.0f;

	// メインマップの半分
	constexpr float SEAHALF_WIDTH = SEAMAP_WIDTH * 0.5f;
	constexpr float SEAHALF_DEPTH = SEAMAP_DEPTH * 0.5f;

	// 砂浜の幅
	constexpr float SEABEACH_WIDTH = 8.0f;

	// 砂浜の外側に向かって下げる高さ
	constexpr float SEABEACH_DROP = 1.2f;

	// 砂浜の1マスの大きさ
	constexpr float SEATILE_SIZE = 1.0f;

	// 砂浜の頂点数
	constexpr int32_t SEAVERTEX_WIDTH = static_cast<int32_t>((SEAMAP_WIDTH + SEABEACH_WIDTH * 2.0f) / SEATILE_SIZE) + 1;

	constexpr int32_t SEAVERTEX_DEPTH = static_cast<int32_t>((SEAMAP_DEPTH + SEABEACH_WIDTH * 2.0f) / SEATILE_SIZE) + 1;

	// 内側の砂浜開始位置
	constexpr int32_t SEAINNER_START = static_cast<int32_t>(SEABEACH_WIDTH / SEATILE_SIZE);

	// メインマップの頂点数
	constexpr int32_t SEAMAIN_VERTEX_WIDTH = static_cast<int32_t>(SEAMAP_WIDTH / SEATILE_SIZE) + 1;

	constexpr int32_t SEAMAIN_VERTEX_DEPTH = static_cast<int32_t>(SEAMAP_DEPTH / SEATILE_SIZE) + 1;

	// 法線計算の許容誤差
	constexpr float SEAEPSILON = 0.0001f;
}  // namespace IslandConfig

// ============================================================
// 島の砂浜データ
// ============================================================

struct SEAMAP_IslandData
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
	Resource<Shader> shader{ "Shader/Terrain.hlsl", BEACH_RASTERIZER_CONFIG };

	// 砂浜専用テクスチャ
	// Resource<Texture> texture{ "Texture/Terrain/Sand.png", true };
	Resource<Texture> texture{ "Texture/Terrain/TileSand.png", true };

	Resource<Material> material{};
	Resource<Surface> surface{};

	Component<Transform> transform{};

	// 砂浜の頂点データ
	std::vector<float> heights{};

	// メインマップを受け取る
	explicit SEAMAP_IslandData(GameObject mainMap);

	// 地形生成
	void Generate(SEAMAP& mainMap);

	// 配列の添字
	int32_t GetIndex(int32_t x, int32_t z) const;
};

// ============================================================
// 島の砂浜
// ============================================================

struct SEAMAP_Island : ObjectBase<SEAMAP_IslandData>
{
	using ObjectBase<SEAMAP_IslandData>::ObjectBase;

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
