#include "pch\pch.h"
#include "Name3.h"
#include <Game/Manager/SessionManager.h>
#include <Game/PlayScene/Player/PlayerJobType.h>

void Name3::Start() {}

void Name3::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	// ヒーラーのプレイヤーIDを取得
	int32_t playerId = -1;

	for (const int32_t id : sessionManager.RefStatus().memberId)
	{
		int32_t roleId = -1;

		if (sessionManager.TryGetPlayerRole(id, roleId) && roleId == PlayerJobType_Healer)
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
