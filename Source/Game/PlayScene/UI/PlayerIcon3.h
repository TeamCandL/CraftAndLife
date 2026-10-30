#pragma once
#include <Game/Game.h>

struct PlayerIcon3Data
{
	Resource<Texture> texture{ "Image/UI/PlayerIcon3.png" };

	Component<UITransform> transform{ Vec2{ 1694, 115 }, Vec2{ 16, 16 } };
	Component<UIImage> image{ texture };
};

struct PlayerIcon3 : ObjectBase<PlayerIcon3Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
