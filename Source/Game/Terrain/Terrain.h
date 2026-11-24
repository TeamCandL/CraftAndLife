#pragma once
#include <Game/Game.h>

/// @brief 地形のデータ
struct TerrainData
{
	/// @brief タイルの4頂点の高さセット
	struct TileHeights
	{
		float h00{};  ///< 左上
		float h10{};  ///< 右上
		float h01{};  ///< 左下
		float h11{};  ///< 右下
	};

#pragma region ほかのパーツと共通化できそうなリソースたち
	Resource<Shader> shader{ "Shader/Terrain.hlsl" };  ///< 地形描画のシェーダ
	Resource<Texture> texture{ "Texture/Terrain/grass_01.png",
		                       //"Texture/Terrain/TileGrass.png",
		                       true,
		                       D3D11_SAMPLER_DESC{
		                           .Filter =
#if 0  // テクスチャをくっきりではなくぼやかすフィルターをつけるか 1/0
				D3D11_FILTER_MIN_MAG_MIP_LINEAR,
#else
		                               D3D11_FILTER_MIN_MAG_MIP_POINT,
#endif
		                           .AddressU       = D3D11_TEXTURE_ADDRESS_WRAP,
		                           .AddressV       = D3D11_TEXTURE_ADDRESS_WRAP,
		                           .AddressW       = D3D11_TEXTURE_ADDRESS_WRAP,
		                           .MipLODBias     = {},
		                           .MaxAnisotropy  = 1,
		                           .ComparisonFunc = D3D11_COMPARISON_NEVER,
		                           .BorderColor    = {},
		                           .MinLOD         = 0.0f,
		                           .MaxLOD         = D3D11_FLOAT32_MAX,
		                       } };  ///< 地形のテクスチャ
	Resource<Material> material{};  ///< 地形のマテリアル
#pragma endregion

	float worldScale{ 200 };  ///< 一時的な地形のサイズ 1面描画を想定(チャンク処理などナシ)

	Resource<Surface> surface{};  ///< 地形の面

	Component<Transform> transform{};  ///< 座標系

	std::vector<uint8_t> rawData{};       ///< 高さ配列 (1次元)
	uint32_t terrainMapResolution{ 32 };  ///< 地形の解像度 多いほどなめらか
	float widthSizeWorld{};               ///< ワールド座標上の1辺のサイズ
	float maxHeight{};                    ///< 最大の高さ
	Vec3 offset{};                        ///< 地形のオフセット
};

/// @brief 地形
struct Terrain : ObjectBase<TerrainData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	/// @brief 地形マップの1辺の頂点数を取得する
	/// @return 地形マップの1辺の頂点数
	uint32_t GetTerrainMapVerticesWidth() const
	{
		return terrainMapResolution + 1;
	}

	/// @brief 指定座標の法線を取得する
	/// @param _worldX ワールドx座標
	/// @param _worldZ ワールドz座標
	/// @return ワールド法線(x,y,z)
	Vec3 GetNormal(const float _worldX, const float _worldZ) const;

	/// @brief 指定座標の高さを取得する
	/// @param _worldX ワールドx座標
	/// @param _worldZ ワールドz座標
	/// @return ワールドy座標
	float GetWorldPositionY(const float _worldX, const float _worldZ) const;

	/// @brief この地形のワールド座標でのオフセットを取得する
	/// @return オフセット(x,y,z)
	Vec3 GetWorldOffset() const;

	/// @brief 地形を指定の値で埋める
	/// @param _resolutionWidthSize 地形の幅
	/// @param _height 埋める高さ
	void FillRaw(const uint32_t _resolutionWidthSize, const uint8_t _height);

	/// @brief 地形を指定の高さで埋める
	/// @param _resolutionWidthSize 地形の幅
	/// @param _worldHeight 埋める高さ(ワールド座標)
	void Fill(const uint32_t _resolutionWidthSize, const float _worldHeight);

  private:
	/// @brief タイルの4頂点の高さを取得する
	/// @param _worldX ワールドx座標
	/// @param _worldZ ワールドz座標
	/// @param _outD タイル小数点以下の位置を出力
	/// @return タイルの4頂点の高さ
	TileHeights GetTileHeights(const float _worldX, const float _worldZ, Vec2* _pOutD) const;

	/// @brief 生のデータにアクセス
	/// @param _indexX インデクス
	/// @param _indexY インデクス
	/// @return 生のデータの参照
	const uint8_t& RawDataAt(const uint32_t _indexX, const uint32_t _indexY) const;

	/// @brief 生のデータにアクセス
	/// @param _indexX インデクス
	/// @param _indexY インデクス
	/// @return 生のデータの参照
	uint8_t& RawDataAt(const uint32_t _indexX, const uint32_t _indexY);

	/// @brief ワールド座標でのXZ座標をRawデータのインデックスに変換
	/// @param _worldX ワールドx座標
	/// @param _worldZ ワールドz座標
	/// @return Rawデータインデクス
	// size_t WorldXZToRawDataIndex(const float _worldX, const float _worldZ) const;

	/// @brief 頂点間の距離を取得する
	/// @return 頂点間の距離
	float GetDistance() const;

	/// @brief 再読み込みする
	void Reload();
};
