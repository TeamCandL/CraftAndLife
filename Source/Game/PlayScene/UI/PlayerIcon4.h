#pragma once
#include <Game/Game.h>

struct PlayerIcon4Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon4.png" };

	Component<UITransform> transform{ Vec2{ 1761, 132 }, Vec2{ 16, 16 } };
	Component<UIImage> image{ texture };
};

struct PlayerIcon4 : ObjectBase<PlayerIcon4Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
