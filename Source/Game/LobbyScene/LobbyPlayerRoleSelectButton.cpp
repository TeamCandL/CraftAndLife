#include "pch\pch.h"
#include "LobbyPlayerRoleSelectButton.h"

#include <Game/Manager/SessionManager.h>

void LobbyPlayerRoleSelectButton::Start()
{
	SessionManager& sessionManager = Get<SessionManager>();
	const auto& status             = sessionManager.RefStatus();

	int playerId = 0;
	if (!sessionManager.TryGetPlayerId(status.name, playerId))
	{
		VCU_LOG("自分プレイヤーID取得失敗");
		return;
	}

	// 自分の役職IDを取得
	if (!sessionManager.TryGetPlayerRole(playerId, currentRoleId))
	{
		VCU_LOG("自分の役職取得失敗: ID={}", playerId);
		return;
	}

	VCU_LOG("自分の役職取得成功: PlayerID={}, RoleID={}", playerId, currentRoleId);
}

void LobbyPlayerRoleSelectButton::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::Right))
	{
	}
}
