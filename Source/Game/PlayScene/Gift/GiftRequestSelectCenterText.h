#pragma once
#include <Game/Game.h>

/// @brief ギフトリクエスト中央の説明テキストのデータ
struct GiftRequestSelectCenterTextData
{
	// 名前のフォント
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(24)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 587 + 217, 167 + 278 }, Vec2{ 312, 189 } };  ///< UI座標系
	Component<UIText> text{ font, Color::FromWebCode(0xffffff) };                        ///< 表示するテキスト内容
};

/// @brief ギフトリクエスト中央の説明テキスト
struct GiftRequestSelectCenterText : ObjectBase<GiftRequestSelectCenterTextData>
{
	void SetText(const std::string& _content);
};
