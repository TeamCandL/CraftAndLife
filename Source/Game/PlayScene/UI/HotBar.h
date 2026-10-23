#pragma once
#include <Game/Game.h>

struct HotBarData
{
	Resource<Texture> texture{ "Image/UI/hot_bar.png" };

	Component<UITransform> transform{ Vec2{ 1206, 908 }, Vec2{ 672, 128 } };
	Component<UIImage> image{ texture };
};

struct TaskBackground : ObjectBase<HotBarData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
