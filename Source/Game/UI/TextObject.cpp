#include "pch\pch.h"
#include "TextObject.h"

void TextObject::SetContent(
    const std::string& _content)
{
	text.content    = _content;
	transform.scale = Vec2{ text.font.Get().pTextFormat->GetFontSize() * _content.size(),
		                    text.font.Get().pTextFormat->GetFontSize() };
}
