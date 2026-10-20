
#pragma once

#include <Game/Game.h>
#include <cstdint>
#include <vector>

/// @brief マップテストシーン
struct MapTestScene : SceneBase
{
	void Start() override;
};

/// @brief マップ生成設定
namespace MapConfig
{
	constexpr int32_t MAP_WIDTH = 200;
	constexpr int32_t MAP_DEPTH = 200;

	constexpr float TILE_SIZE = 1.0f;
	constexpr float BASE_HEIGHT = 0.0f;
	constexpr float MAX_HEIGHT = 12.0f;

	constexpr float PATH_WIDTH = 5.0f;
	constexpr float RIVER_WIDTH = 4.0f;

	constexpr float SMOOTHING_RATE = 0.25f;
	constexpr int32_t SMOOTHING_ITERATIONS = 3;
} // namespace MapConfig

/// @brief マップデータ
struct MapData
{
	MapData();

	// 描画リソース
	Resource<Shader> shader{"Shader/Terrain.hlsl"};

	Resource<Texture> texture{
	    "Texture/Terrain/TileGrass.png", true,
	    D3D11_SAMPLER_DESC{
	        .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
	        .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
	        .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
	        .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	        .MipLODBias = {},
	        .MaxAnisotropy = 1,
	        .ComparisonFunc = D3D11_COMPARISON_NEVER,
	        .BorderColor = {},
	        .MinLOD = 0.0f,
	        .MaxLOD = D3D11_FLOAT32_MAX,
	    }};

	Resource<Material> material{};
	Resource<Surface> surface{};

	// 座標系
	Component<Transform> transform{};

	// 地形の高さデータ
	std::vector<float> heights{};

	// 地形生成
	void Generate();
	void SmoothHeights();

	// 地形情報取得
	int32_t GetIndex(int32_t x, int32_t z) const;
	float GetHeight(float x, float z) const;
	bool IsInsideMap(float x, float z) const;

	// 道・川の判定
	bool IsPath(float x, float z) const;
	bool IsRiver(float x, float z) const;

	// スタート・ゴール
	Vec3 GetStartPosition() const;
	Vec3 GetGoalPosition() const;

private:
	float CalculateHeight(float x, float z) const;
};

/// @brief マップオブジェクト
struct MAP : ObjectBase<MapData>
{
	using ObjectBase<MapData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

private:
	/// @brief 地形メッシュを再生成する
	void Reload();
};
