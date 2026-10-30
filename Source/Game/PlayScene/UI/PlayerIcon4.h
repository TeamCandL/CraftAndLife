#pragma once
#include <Game/Game.h>

struct PlayerIcon4Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon4.png" };

	Component<UITransform> transform{ Vec2{ 1688, 199 }, Vec2{ 16, 16 } };

	Component<UIImage> image{ texture };

	Vec2 miniMapCenterPosition{ 1746, 174 };

	bool isActivePlayer{ false };
};

struct PlayerIcon4 : ObjectBase<PlayerIcon4Data>
{
	using ObjectBase<PlayerIcon4Data>::ObjectBase;

	void Start() override;
	void Update() override;

	void SetPosition(const float _angle, const float _radius);

	void SetActivePlayer(
	    const bool _isActivePlayer)
	{
		isActivePlayer = _isActivePlayer;
	}
};