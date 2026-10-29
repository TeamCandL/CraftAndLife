#pragma once
#include <Game/Game.h>

struct BossGaugeData
{
	Resource<Texture> texture{ "Image/UI/BossGauge.png" };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIImage> image{ texture };
};

struct BossGauge : ObjectBase<BossGaugeData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
