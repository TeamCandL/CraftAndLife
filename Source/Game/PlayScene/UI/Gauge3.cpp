#include "pch\pch.h"
#include "Gauge3.h"

#include "../Player.h"
#include "../OtherPlayer.h"
#include "../OtherPlayerController.h"

#include <Game/Manager/SessionManager.h>
#include <Game/PlayScene/Player/PlayerJobType.h>

void Gauge3::Start()
{
	transform.scale.x = maxWidth;
}

void Gauge3::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	// HealerのプレイヤーIDを取得
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
	{
		transform.scale.x = 0.0f;
		return;
	}

	std::string playerName;

	if (!sessionManager.TryGetPlayerName(playerId, playerName))
	{
		transform.scale.x = 0.0f;
		return;
	}

	int32_t hp    = 0;
	int32_t maxHP = 0;

	if (playerName == sessionManager.RefStatus().name)
	{
		Player& player = Get<Player>(objPlayer);

		hp    = player.Data().hp;
		maxHP = player.Data().maxHP;
	}
	else
	{
		OtherPlayerController& controller = Get<OtherPlayerController>(objOtherPlayerController);

		GameObject objOtherPlayer{};

		if (!controller.TryGetOtherPlayer(playerName, objOtherPlayer))
		{
			transform.scale.x = 0.0f;
			return;
		}

		OtherPlayer& otherPlayer = Get<OtherPlayer>(objOtherPlayer);

		hp    = otherPlayer.GetHP();
		maxHP = otherPlayer.GetMaxHP();
	}

	if (maxHP <= 0)
	{
		transform.scale.x = 0.0f;
		return;
	}

	float hpRate = static_cast<float>(hp) / static_cast<float>(maxHP);

	if (hpRate < 0.0f)
		hpRate = 0.0f;

	if (hpRate > 1.0f)
		hpRate = 1.0f;

	transform.scale.x = maxWidth * hpRate;
}
