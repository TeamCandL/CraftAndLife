#pragma once
#pragma once
#pragma once
#include <Game/Game.h>

struct CreatePlayerNameData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 115, 207 }, Vec2{ 1430, 93 } };
	Component<UIText> text{ font,
		                    Color::WHITE,
		                    "プレイヤー名を入力してください" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct CreatePlayerName : ObjectBase<CreatePlayerNameData>
{
	// とくに書かなくともok
	void Start() override;
};
