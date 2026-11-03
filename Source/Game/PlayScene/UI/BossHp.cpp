#include "pch\pch.h"
#include "BossHp.h"
#include <Game/Manager/SessionManager.h>
#include "../Enemy.h"

void BossHp::Start()
{
	isBossActive = false;
}

void BossHp::Update()
{
	if (isBossActive)
	{
		Enemy& boss  = Get<Enemy>(objBoss);
		hp           = boss.Data().hp;
		rateOfHp     = static_cast<float>(hp) / static_cast<float>(maxHP);
		text.content = "%d / %d", hp, maxHP;

		if (hp <= 0)
		{
			isBossActive = false;
		}
	}
}
