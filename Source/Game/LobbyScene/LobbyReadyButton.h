#pragma once
#include <Game/Game.h>

/// @brief ロビー準備okボタンのデータ
struct LobbyReadyButtonData
{
	std::string_view textureReadyFileName{ "Image/Lobby/ReadyButton.png" };
	std::string_view textureWaitingFileName{ "Image/Lobby/WaitingButton.png" };
	std::string_view textureGameStartFileName{ "Image/Lobby/GameStartButton.png" };

	GameObject objButton{};  ///< ボタンオブジェクト
	Vec2 begin{ 720, 840 };  ///< ボタンの開始位置ピクセル
	Vec2 scale{ 482, 142 };  ///< ボタンのサイズピクセル

	bool isStatable{ false };
};

/// @brief ロビー準備okボタン
struct LobbyReadyButton : ObjectBase<LobbyReadyButtonData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
