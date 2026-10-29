#pragma once
#include <Game/Game.h>

struct TitleLogoData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 192, 237 }, Vec2{ 459, 104 } };
	Component<UIText> text{ font, Color::WHITE };
};

struct TitleLogo : ObjectBase<TitleLogoData>
{
};
