#pragma once
#include <Game/Game.h>

struct MyGaugeData
{
	MyGaugeData(
	    GameObject _objPlayer) :
	    objPlayer{ _objPlayer }
	{
	}

	GameObject objPlayer{};

	Resource<Texture> texture{ "Image/UI/MyGauge.png" };

	const float maxWidth{ 423.0f };

	Component<UITransform> transform{ Vec2{ 265, 987 }, Vec2{ 423, 31 } };

	Component<UIImage> image{ texture };
};

struct MyGauge : ObjectBase<MyGaugeData>
{
	using ObjectBase<MyGaugeData>::ObjectBase;

	void Start() override;
	void Update() override;
};
