#pragma once
#include <Base/Base.h>
// #include "Base/Core/Resource/ResourceType.h"
// #include "Base/Core/Resource.h"
// #include "Base/Core/Resource/Font.h"

/// @brief スプライトの描画コンポーネント
struct UIText : ComponentBase<UIText>
{
	UIText(
	    const std::string_view _content = "") :
	    content{ _content }
	{
	}

	UIText(
	    Resource<ResourceType::Font> _font,
	    const std::string_view _content = "") :
	    content{ _content },
	    font{ _font }
	{
	}

	Resource<ResourceType::Font> font{ "Font/YuseiMagic/YuseiMagic-Regular.ttf",
		                               L"Yusei Magic",
		                               32.0f,
		                               L"ja-jp" };  ///< 表示するテキストのフォント
	std::string content{};                          ///< 表示したいテキスト

	static inline Resource<ResourceType::Font> defaultFont{};
};
