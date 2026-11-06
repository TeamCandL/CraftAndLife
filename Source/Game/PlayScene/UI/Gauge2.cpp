
#include "pch\pch.h"
#include "Gauge2.h"

#include "../Player.h"
#include "../OtherPlayer.h"
#include "../OtherPlayerController.h"

#include <Game/Manager/SessionManager.h>

void Gauge2::Start()
{
	transform.scale.x = maxWidth;
}

void Gauge2::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	std::string playerName;

	// 担当プレイヤーの名前を取得
	if (!sessionManager.TryGetPlayerName(playerId, playerName))
	{
		transform.scale.x = 0.0f;
		return;
	}

	int32_t hp    = 0;
	int32_t maxHP = 0;

	// 自分自身の場合
	if (playerName == sessionManager.RefStatus().name)
	{
		Player& player = Get<Player>(objPlayer);

		hp    = player.Data().hp;
		maxHP = player.Data().maxHP;
	}
	// 他プレイヤーの場合
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
