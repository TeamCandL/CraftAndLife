#pragma once
#include <Game/Game.h>

struct Gage3Data
{
	Resource<Texture> texture{ "Image/UI/Gage3.png" };

	Component<UITransform> transform{ Vec2{ 27, 585 }, Vec2{ 192, 24 } };
	Component<UIImage> image{ texture };
};

struct Gage3 : ObjectBase<Gage3Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
