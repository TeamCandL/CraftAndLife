#pragma once
#include <Game/Game.h>

/// @brief チャットログのテキストUI
struct ChatTextLogData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_BottomRight) };

	Component<UITransform> transform{ Vec2{ 1288, 363 }, Vec2{ 632, 531 }, UILayerOrder{}.WithUser(105) };
	Component<UIText> text{ font, Color::FromWebCode(0x000000) };

	std::vector<std::pair<float, std::string>> logs{};
	std::vector<std::string> textChatRequests{};
	const float SHOW_TIME{ 6.0f };

	// 通知音
	Resource<Audio> noticeSE{ "BGM_SE/Notice.mp3" };

	GameObject objChatTextLogBack{};
};

struct ChatTextLog : ObjectBase<ChatTextLogData>
{
	void Start() override;
	void Update() override;

	// 通知音を鳴らすか指定できる
	void AddLog(const std::string& _content, bool _playNotice = true);

	void AddLogRequest(
	    const std::string& _content)
	{
		textChatRequests.emplace_back(_content);
	}

	std::vector<std::string> TakeLogRequests()
	{
		return std::exchange(textChatRequests, {});
	}
};