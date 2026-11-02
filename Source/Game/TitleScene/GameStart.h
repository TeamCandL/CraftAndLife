#pragma once
#pragma once
#pragma once
#include <Game/Game.h>

struct GameStartData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 717, 217 }, Vec2{ 485, 93 } };
	Component<UIText> text{ font, Color::WHITE, "ゲームスタート!" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct GameStart : ObjectBase<GameStartData>
{
	// とくに書かなくともok
	void Start() override;
};
