#pragma once
#include <Game/Game.h>

struct PlayerIcon2Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon2.png" };

	Component<UITransform> transform{ Vec2{ 1688, 199 }, Vec2{ 16, 16 } };
	Component<UIImage> image{ texture };
};

struct PlayerIcon2 : ObjectBase<PlayerIcon2Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
