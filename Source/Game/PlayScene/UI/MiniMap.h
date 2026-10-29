#pragma once
#include <Game/Game.h>

struct MiniMapData
{
	MiniMapData(
	    GameObject _objPlayer)
	{
		objPlayer = _objPlayer;
	}
	Resource<Texture> texture{ "Image/UI/mini_map.png" };  ///< ミニマップの背景画像

	Component<UITransform> transform{ Vec2{ 1572, 0 }, Vec2{ 348, 348 }, UILayerOrder{}.WithUser(0) };  ///< UI座標系
	Component<UIImage> image{ texture };  ///< 画像テクスチャ

	GameObject objPlayer{};
	std::vector<GameObject> points{};  ///< ミニマップに表示するマーク
};

struct MiniMap : ObjectBase<MiniMapData>
{
	using ObjectBase<MiniMapData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
