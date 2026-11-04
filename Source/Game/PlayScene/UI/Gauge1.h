#pragma once
#include <Game/Game.h>

struct Gauge1Data
{
	Resource<Texture> texture{ "Image/UI/Gauge1.png" };

	Component<UITransform> transform{ Vec2{ 27, 423 }, Vec2{ 192, 24 } };
	Component<UIImage> image{ texture };
};

struct Gauge1 : ObjectBase<Gauge1Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
