#include "pch\pch.h"
#include "Gauge1.h"

#include "../Player.h"
#include "../OtherPlayer.h"
#include "../OtherPlayerController.h"

#include <Game/Manager/SessionManager.h>

void Gauge1::Start()
{
	transform.scale.x = maxWidth;
}

void Gauge1::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	// プレイヤーIDから名前を取得
	std::string playerName;

	if (!sessionManager.TryGetPlayerName(playerId, playerName))
	{
		transform.scale.x = 0.0f;
		return;
	}

	int32_t hp    = 0;
	int32_t maxHP = 0;

	// 担当プレイヤーが自分の場合
	if (playerName == sessionManager.RefStatus().name)
	{
		Player& player = Get<Player>(objPlayer);

		hp    = player.Data().hp;
		maxHP = player.Data().maxHP;
	}
	// 担当プレイヤーが他人の場合
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

	// 最大HPが0以下なら計算しない
	if (maxHP <= 0)
	{
		transform.scale.x = 0.0f;
		return;
	}

	// HPの割合を計算
	float hpRate = static_cast<float>(hp) / static_cast<float>(maxHP);

	// 0～1の範囲に制限
	if (hpRate < 0.0f)
		hpRate = 0.0f;

	if (hpRate > 1.0f)
		hpRate = 1.0f;

	// HPに応じてゲージ幅を変更
	transform.scale.x = maxWidth * hpRate;
}