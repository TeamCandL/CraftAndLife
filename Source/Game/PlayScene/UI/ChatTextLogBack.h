#pragma once
#include <Game/Game.h>

/// @brief チャットログのテキストUI
struct ChatTextLogBackData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_BottomRight) };

	Component<UITransform> transform{ Vec2{ 1288 + 2, 363 + 2 },
		                              Vec2{ 632, 531 },
		                              UILayerOrder{}.WithUser(100) };  ///< UIの座標系
	Component<UIText> text{ font, Color::FromWebCode(0xffffff) };      ///< UIのテキスト
};

/// @brief チャットログのテキストUI
struct ChatTextLogBack : ObjectBase<ChatTextLogBackData>
{
};
