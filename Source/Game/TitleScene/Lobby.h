#pragma once
#pragma once
#include <Game/Game.h>

struct LobbyData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 61, 0 }, Vec2{ 211, 93 } };
	Component<UIText> text{ font, Color::WHITE, "ロビー" };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct Lobby : ObjectBase<LobbyData>
{
	// とくに書かなくともok
	void Start() override;
};
