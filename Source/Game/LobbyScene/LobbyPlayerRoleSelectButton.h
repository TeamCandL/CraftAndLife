#pragma once
#include <Game/Game.h>

/// @brief プレイヤーのロールセレクトボタンのデータ
struct LobbyPlayerRoleSelectButtonData
{
	int currentRoleId{};
	bool isRoleChange{ false };
	bool isInitialized{ false };
};

/// @brief プレイヤーのロールセレクトボタン
struct LobbyPlayerRoleSelectButton : ObjectBase<LobbyPlayerRoleSelectButtonData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	int GetCurrentRoleId() const
	{
		return currentRoleId;
	}

	bool IsInitialized() const
	{
		return isInitialized;
	}
};