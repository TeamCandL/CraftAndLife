#pragma once
#include <Game/Game.h>

struct Gage2Data
{
	Resource<Texture> texture{ "Image/UI/Gage2.png" };

	Component<UITransform> transform{ Vec2{ 27, 504 }, Vec2{ 192, 24 } };
	Component<UIImage> image{ texture };
};

struct Gage2 : ObjectBase<Gage2Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
