#pragma once
#include <Game/Game.h>

struct TestImageCircleData
{
	Resource<Texture> texture{ "Image/UI/Gift/gift_request_item_background.png" };

	Component<UITransform> transform{ Vec2{ 587, 167 }, Vec2{ 746, 746 }, UILayerOrder{}.WithUser(10020) };
	Component<UIImageCircle> image{ texture };
};

struct TestImageCircle : ObjectBase<TestImageCircleData>
{
	void Start() override
	{
		image.minRadius = 161.0f;
		image.maxRadius = 374.0f;
	}
	void Update() override;
};
