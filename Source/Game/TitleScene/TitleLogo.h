#pragma once
#include <Game/Game.h>

/// @brief タイトルロゴのデータ
struct TitleLogoData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 350, 141 }, Vec2{ 459, 104 } };
	Component<UIText> text{ font, Color::WHITE, "Craft&Life" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

/// @brief タイトルロゴ
struct TitleLogo : ObjectBase<TitleLogoData>
{
	/// @brief 開始処理
	void Start() override;
};
