#pragma once
#include <Game/Game.h>

struct DeathUIData
{
	// 対象となるプレイヤー
	GameObject targetPlayer{};

	// 自分のプレイヤーかどうか
	bool isLocalPlayer{ false };

	// 表示画像
	Resource<Texture> texture{ "Image/UI/Death.png" };

	// UIの位置と大きさ
	Component<UITransform> transform{ Vec2{ 0, 0 }, Vec2{ 70, 83 } };

	Component<UIImage> image{ texture };

	// コンストラクタ
	DeathUIData(
	    GameObject _targetPlayer,
	    bool _isLocalPlayer = false) :
	    targetPlayer{ _targetPlayer },
	    isLocalPlayer{ _isLocalPlayer }
	{
	}
};

struct DeathUI : ObjectBase<DeathUIData>
{
	using ObjectBase<DeathUIData>::ObjectBase;

	void Start() override;
	void Update() override;
};