#pragma once
#include <Game/Game.h>

struct MiniMapData
{
	Resource<Texture> texture{ "Image/UI/task_background.png" };

	Component<UITransform> transform{ Vec2{ 1572, 0 }, Vec2{ 348, 348 } };
	Component<UIImage> image{ texture };
};

struct TaskBackground : ObjectBase<MiniMapData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
