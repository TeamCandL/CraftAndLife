#pragma once
#pragma once
#include <Game/Game.h>

struct LobbyTeamNameData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)  // ここがフォントサイズ
		                     .WithFontAlignment(
		                         FontAlignment_Center) };  // ここが配置(LeftとかRightとかTopとかRightBottomとかあるよ

	// 1つめに座標のVec2, 2つ目にスケールのVec2を入れる
	Component<UITransform> transform{ Vec2{ 61, 93 }, Vec2{ 725, 93 } };
	Component<UIText> text{ font, Color::WHITE, "チーム名: " };  // 1つ目がフォントリソース、２つ目がテキストの色
};

struct LobbyTeamName : ObjectBase<LobbyTeamNameData>
{
	// とくに書かなくともok
	void Start() override;
};
