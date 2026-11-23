#include "pch\pch.h"
#include "LobbyPlayerRoleSelectButton.h"
#include <Game/PlayScene/Player/PlayerJobType.h>

#include <Game/Manager/SessionManager.h>

void LobbyPlayerRoleSelectButton::Start() {}

void LobbyPlayerRoleSelectButton::Update()
{
	if (!isInitialized)
	{
		SessionManager& session = Get<SessionManager>();
		const auto& status      = session.RefStatus();

		int playerId = 0;

		// 自分のプレイヤーIDを取得
		if (!session.TryGetPlayerId(status.name, playerId))
			return;

		// 自分の役職IDを取得
		if (!session.TryGetPlayerRole(playerId, currentRoleId))
			return;

		isInitialized = true;

		VCU_LOG("役職初期化完了: ID={}, RoleID={}", playerId, currentRoleId);
	}

	if (Get<InputSystem>().GetKeyDown(KeyCode::D))
	{
		currentRoleId++;

		if (currentRoleId >= static_cast<int>(PlayerJobType_Max))
		{
			currentRoleId = 0;
		}

		isRoleChange = true;
		VCU_LOG("役職変更: RoleID={}", currentRoleId);
	}
	else if (Get<InputSystem>().GetKeyDown(KeyCode::A))
	{
		// 前の役職
		currentRoleId--;

		if (currentRoleId < 0)
		{
			currentRoleId = static_cast<int>(PlayerJobType_Max) - 1;
		}

		isRoleChange = true;
		VCU_LOG("役職変更: RoleID={}", currentRoleId);
	}

	if (isRoleChange)
	{
		// ここで役職変更プロトコルを送信。まだ未実装

		VCU_LOG("役職変更送信: RoleID={}", currentRoleId);

		isRoleChange = false;
	}
}
