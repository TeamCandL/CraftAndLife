#pragma once
#include <Game/Game.h>

struct Name4Data
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Left) };

	Component<UITransform> transform{ Vec2{ 90, 615 }, Vec2{ 192, 27 }, UILayerOrder{}.WithUser(10000) };
	Component<UIText> text{ font, Color::WHITE };
};

struct Name4 : ObjectBase<Name4Data>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
