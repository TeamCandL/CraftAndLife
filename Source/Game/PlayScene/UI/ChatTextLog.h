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

	Component<UITransform> transform{ Vec2{ 1288, 363 },
		                              Vec2{ 632, 531 },
		                              UILayerOrder{}.WithUser(105) };  ///< UIの座標系
	Component<UIText> text{ font, Color::FromWebCode(0x000000) };      ///< UIのテキスト

	std::vector<std::pair<float, std::string>> logs{};  ///< チャットのログ
	std::vector<std::string> textChatRequests{};
	const float SHOW_TIME{ 6.0f };

	GameObject objChatTextLogBack{};  ///< 裏の文字オブジェクト
};

/// @brief チャットログのテキストUI
struct ChatTextLog : ObjectBase<ChatTextLogData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief ログを追加する
	/// @param _content チャットの内容
	void AddLog(const std::string& _content);

	/// @brief ログ追加リクエスト
	/// @param _content チャットの内容
	void AddLogRequest(
	    const std::string& _content)
	{
		textChatRequests.emplace_back(_content);
	}
	/// @brief ログ追加リクエストを一括取得
	/// @return
	std::vector<std::string> TakeLogRequests()
	{
		return std::exchange(textChatRequests, {});
	}
};
