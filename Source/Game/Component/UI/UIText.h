#pragma once
#include <Base/Base.h>
// #include "Base/Core/Resource/ResourceType.h"
// #include "Base/Core/Resource.h"
// #include "Base/Core/Resource/Font.h"

/// @brief スプライトの描画コンポーネント
struct UIText : ComponentBase<UIText>
{
	UIText(
	    // 文字だけ描画したいお客様むけ
	    const std::string_view _content = "",
	    const Color _color              = Color::BLACK) :
	    font{ "Font/YuseiMagic/YuseiMagic-Regular.ttf", L"Yusei Magic", 32.0f, L"ja-jp" },
	    color{ _color },
	    content{ _content }
	{
	}

	UIText(
	    // 動的に文字を描画したいお客様向け
	    Resource<ResourceType::Font> _font,
	    const Color _color              = Color::BLACK,
	    const std::string_view _content = "") :
	    font{ _font },
	    color{ _color },
	    content{ _content }
	{
	}

	Resource<ResourceType::Font> font{};  ///< 表示するテキストのフォント
	Color color{ Color::BLACK };          ///< 描画する文字の色
	std::string content{};                ///< 表示したいテキスト
};
