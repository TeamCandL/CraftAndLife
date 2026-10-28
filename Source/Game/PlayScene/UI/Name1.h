#pragma once
#include <Game/Game.h>

struct Name1Data
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Left) };

	Component<UITransform> transform{ Vec2{ 272, 1309 }, Vec2{ 192, 27 } };
	Component<UIText> text{ font, Color::WHITE };
};

struct Name1 : ObjectBase<Name1Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
