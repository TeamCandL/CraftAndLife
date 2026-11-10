#pragma once
#include <Game/Game.h>

struct TaskBackgroundData
{
	TaskBackgroundData(
	    GameObject objDayContloller) :
	    objDayContloller{ objDayContloller }
	{
	}
	Resource<Texture> texture{ "Image/UI/task_background.png" };
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 0, 0 }, Vec2{ 520, 130 } };
	Component<UIImage> image{ texture };
	Component<UIText> text{ font, Color::WHITE };
	GameObject objDayContloller{};
};

struct TaskBackground : ObjectBase<TaskBackgroundData>
{
	using ObjectBase<TaskBackgroundData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
