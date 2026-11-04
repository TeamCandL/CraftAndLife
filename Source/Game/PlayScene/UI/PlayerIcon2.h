#pragma once
#include <Game/Game.h>

struct PlayerIcon2Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon2.png" };

	Component<UITransform> transform{ Vec2{ 1688, 199 }, Vec2{ 16, 16 } };

	Component<UIImage> image{ texture };

	// ミニマップの中心座標
	Vec2 miniMapCenterPosition{ 1746, 174 };

	bool isActivePlayer{ false };
};

struct PlayerIcon2 : ObjectBase<PlayerIcon2Data>
{
	using ObjectBase<PlayerIcon2Data>::ObjectBase;

	void Start() override;
	void Update() override;

	// ミニマップ上の位置を設定
	void SetPosition(const float _angle, const float _radius);

	void SetActivePlayer(
	    const bool _isActivePlayer)
	{
		isActivePlayer = _isActivePlayer;
	}
};