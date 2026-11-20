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

	GameObject objEnemyController{};

	GameObject objOtherEnemyController{};

	GameObject objPlayerIcon2{};
	GameObject objPlayerIcon3{};
	GameObject objPlayerIcon4{};

	std::vector<GameObject> points{};

	// 敵アイコン管理
	std::vector<GameObject> enemyIcons{};
	std::vector<bool> enemyIconIsBoss{};
};

struct MiniMap : ObjectBase<MiniMapData>
{
	using ObjectBase<MiniMapData>::ObjectBase;

	void Start() override;
	void Update() override;

	void SetEnemyController(GameObject obj);

	void SetOtherEnemyController(GameObject obj);
};