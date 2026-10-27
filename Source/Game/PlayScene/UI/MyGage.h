#pragma once
#include <Game/Game.h>

struct MyGageData
{
	MyGageData(
	    GameObject _objPlayer) :
	    objPlayer{ _objPlayer }
	{
	}

	GameObject objPlayer{};

	Resource<Texture> texture{ "Image/UI/MyGage.png" };

	const float maxWidth{ 423.0f };

	Component<UITransform> transform{ Vec2{ 265, 987 }, Vec2{ 423, 31 } };

	Component<UIImage> image{ texture };
};

struct MyGage : ObjectBase<MyGageData>
{
	using ObjectBase<MyGageData>::ObjectBase;

	void Start() override;
	void Update() override;
};
