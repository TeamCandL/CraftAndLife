#pragma once
#include <Game/Game.h>

struct MiniMapData
{
	MiniMapData(
	    GameObject _objPlayer,
	    GameObject _objOtherPlayerController)
	{
		objPlayer                = _objPlayer;
		objOtherPlayerController = _objOtherPlayerController;
	}

	Resource<Texture> texture{ "Image/UI/mini_map.png" };

	Component<UITransform> transform{ Vec2{ 1572, 0 }, Vec2{ 348, 348 }, UILayerOrder{}.WithUser(0) };

	Component<UIImage> image{ texture };

	GameObject objPlayer{};
	GameObject objOtherPlayerController{};

	GameObject objPlayerIcon2{};
	GameObject objPlayerIcon3{};
	GameObject objPlayerIcon4{};

	std::vector<GameObject> points{};
};

struct MiniMap : ObjectBase<MiniMapData>
{
	using ObjectBase<MiniMapData>::ObjectBase;

	void Start() override;
	void Update() override;
};