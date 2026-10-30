#pragma once
#include <Game/Game.h>

struct TitleLogoData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 350, 141 }, Vec2{ 459, 104 } };
	Component<UIText> text{ font, Color::WHITE, "Craft&Life" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct TitleLogo : ObjectBase<TitleLogoData>
{
	// とくに書かなくともok
	void Start() override;
};
