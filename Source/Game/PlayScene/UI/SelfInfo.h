#pragma once
#include <Game/Game.h>

struct SelfInfoData
{
	Resource<Texture> texture{ "Image/UI/hot_bar.png" };

	Component<UITransform> transform{ Vec2{ 245, 917 }, Vec2{ 464, 120 } };
	Component<UIImage> image{ texture };
};

struct TaskBackground : ObjectBase<SelfInfoData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
