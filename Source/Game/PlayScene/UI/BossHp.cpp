#include "pch\pch.h"
#include "BossHp.h"
#include <Game/Manager/SessionManager.h>
#include "../Enemy.h"
#include "BossInfo.h"

void BossHp::Start()
{
	SetActive(false);
}

void BossHp::Update()
{
	SetActive(Get<BossInfo>(objBossInfo).IsBossActive());
	if (IsActive())
	{
		HpInfo hpInfo = Get<BossInfo>(objBossInfo).GetHpInfo();
		text.content  = std::to_string(hpInfo.hp) + " / " + std::to_string(hpInfo.maxHP);
	}
}
