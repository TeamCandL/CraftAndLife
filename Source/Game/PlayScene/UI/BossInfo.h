#pragma once
#include <Game/Game.h>

struct BossInfoData
{
	Resource<Texture> texture{ "Image/UI/boss_info.png" };

	Component<UITransform> transform{ Vec2{ 657, 0 }, Vec2{ 577, 100 } };
	Component<UIImage> image{ texture };
};

struct BossInfo : ObjectBase<BossInfoData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
