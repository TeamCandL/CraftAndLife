#pragma once
#include <Game/Game.h>

struct MyNameData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Left) };

	Component<UITransform> transform{ Vec2{ 265, 917 }, Vec2{ 412, 69 } };
	Component<UIText> text{ font, Color::WHITE };
};

struct MyName : ObjectBase<MyNameData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
