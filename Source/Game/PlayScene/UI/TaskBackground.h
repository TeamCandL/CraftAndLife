#pragma once
#include <Game/Game.h>

struct TaskBackgroundData
{
	Resource<Texture> texture{ "Image/UI/task_background.png" };

	Component<UITransform> transform{ Vec2{ 0, 0 }, Vec2{ 520, 130 } };
	Component<UIImage> image{ texture };
};

struct TaskBackground : ObjectBase<TaskBackgroundData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
