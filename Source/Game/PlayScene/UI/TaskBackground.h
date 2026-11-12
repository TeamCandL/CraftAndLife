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
	bool isEnabledDayTimer{};  ///< 残り時間と日付の表示が有効であるか true / false
};

struct TaskBackground : ObjectBase<TaskBackgroundData>
{
	using ObjectBase<TaskBackgroundData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief タスク内容を設定する
	/// @param _taskContent タスク内容の文字列
	void SetTask(const std::string& _taskContent);

	/// @brief 残り時間と日付の表示を有効にする
	/// @param _enabled 有効 true / false 無効
	void SetEnableDayTimer(
	    const bool _enabled)
	{
		isEnabledDayTimer = _enabled;
	}
};
