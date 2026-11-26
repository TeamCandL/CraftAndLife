#pragma once
#include <Game/Game.h>

struct OtherInfoData
{
	Resource<Texture> texture{ "Image/UI/other_info.png" };

	Component<UITransform> transform{ Vec2{ 0, 367 }, Vec2{ 314, 334 } };
	Component<UIImage> image{ texture };
};

struct OtherInfo : ObjectBase<OtherInfoData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
