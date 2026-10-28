#pragma once
#include <Game/Game.h>

struct BossGageRData
{
	Resource<Texture> texture{ "Image/UI/BossGageR.png" };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIImage> image{ texture };
};

struct BossGageR : ObjectBase<BossGageRData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
