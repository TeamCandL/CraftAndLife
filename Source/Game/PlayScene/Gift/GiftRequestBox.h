#pragma once
#include <Game/Game.h>
#include <Game/Component/UI/UIWorldBinder.h>

struct GiftRequestBoxData
{
	Resource<Texture> background{ "Image/UI/Gift/message_box.png" };

	Component<UITransform> uiTransform{ Vec2{ 82.0f * -0.5f, 103.0f * -1.0f }, Vec2{ 82, 103 } };
	Component<Transform> transform{};
	Component<UIWorldBinder> uiWorldBinder{ uiTransform, transform, 10.0f };

	Component<UIImage> imageBackground{ background };
};
struct GiftRequestBox : ObjectBase<GiftRequestBoxData>
{
};
