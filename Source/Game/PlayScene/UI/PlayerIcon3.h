#pragma once
#include <Game/Game.h>

struct PlayerIcon3Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon3.png" };

	Component<UITransform> transform{ Vec2{ 1688, 199 }, Vec2{ 16, 16 } };

	Component<UIImage> image{ texture };

	Vec2 miniMapCenterPosition{ 1746, 174 };
};

struct PlayerIcon3 : ObjectBase<PlayerIcon3Data>
{
	using ObjectBase<PlayerIcon3Data>::ObjectBase;

	void Start() override;
	void Update() override;

	void SetPosition(const float _angle, const float _radius);
};