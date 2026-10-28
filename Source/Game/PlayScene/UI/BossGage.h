#pragma once
#include <Game/Game.h>

struct BossGageData
{
	Resource<Texture> texture{ "Image/UI/BossGage.png" };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIImage> image{ texture };
};

struct BossGage : ObjectBase<BossGageData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
