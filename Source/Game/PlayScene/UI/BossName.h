#pragma once
#include <Game/Game.h>

struct BossNameData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 657, 0 }, Vec2{ 577, 50 } };
	Component<UIText> text{ font, Color::WHITE };
};

struct BossName : ObjectBase<BossNameData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
