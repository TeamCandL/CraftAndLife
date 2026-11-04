#pragma once
#include <Game/Game.h>

struct TitleLogoStartGuideData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 651, 339 }, Vec2{ 617, 339 } };
	Component<UIText> text{ font, Color::WHITE, "Aボタンでスタート" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct TitleLogoStartGuide : ObjectBase<TitleLogoStartGuideData>
{
	// とくに書かなくともok
	void Start() override;
};
