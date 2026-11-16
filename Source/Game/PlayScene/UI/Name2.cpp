#include "pch\pch.h"
#include "Name2.h"
#include <Game/Manager/SessionManager.h>
#include <Game/PlayScene/Player/PlayerJobType.h>

void Name2::Start() {}

void Name2::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	// ヒーラーのプレイヤーIDを取得
	int32_t playerId = -1;

	for (const int32_t id : sessionManager.RefStatus().memberId)
	{
		int32_t roleId = -1;

		if (sessionManager.TryGetPlayerRole(id, roleId) && roleId == PlayerJobType_Tank)
		{
			playerId = id;
			break;
		}
	}

	if (playerId == -1)
		return;

	// プレイヤーIDから名前を取得
	std::string playerName;

	if (!sessionManager.TryGetPlayerName(playerId, playerName))
		return;

	text.content = playerName;
}
