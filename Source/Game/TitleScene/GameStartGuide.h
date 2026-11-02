#pragma once
#pragma once
#include <Game/Game.h>

struct GameStartGuideData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 576, 540 }, Vec2{ 767, 186 } };
	Component<UIText> text{ font, Color::WHITE, "選択ジョブ: " };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct GameStartGuide : ObjectBase<GameStartGuideData>
{
	// とくに書かなくともok
	void Start() override;
};
