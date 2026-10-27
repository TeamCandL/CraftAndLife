#pragma once
#include <Game/Game.h>

struct Gage4Data
{
	Resource<Texture> texture{ "Image/UI/Gage4.png" };

	Component<UITransform> transform{ Vec2{ 27, 666 }, Vec2{ 192, 24 } };
	Component<UIImage> image{ texture };
};

struct Gage4 : ObjectBase<Gage4Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
