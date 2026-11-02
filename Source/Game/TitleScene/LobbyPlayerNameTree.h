#pragma once
#pragma once
#include <Game/Game.h>

struct LobbyPlayerNameTreeData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 61, 930 }, Vec2{ 1108, 93 } };
	Component<UIText> text{ font,
		                    Color::WHITE,
		                    "name 準備中 ジョブ: " };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct LobbyPlayerNameTree : ObjectBase<LobbyPlayerNameTreeData>
{
	// とくに書かなくともok
	void Start() override;
};
