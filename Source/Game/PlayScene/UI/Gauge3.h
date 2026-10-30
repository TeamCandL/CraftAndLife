#pragma once
#include <Game/Game.h>

struct Gauge3Data
{
	Resource<Texture> texture{ "Image/UI/Gauge3.png" };

	Component<UITransform> transform{ Vec2{ 27, 585 }, Vec2{ 192, 24 } };
	Component<UIImage> image{ texture };
};

struct Gauge3 : ObjectBase<Gauge3Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
