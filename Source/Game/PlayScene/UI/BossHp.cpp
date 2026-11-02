#include "pch\pch.h"
#include "BossHp.h"
#include <Game/Manager/SessionManager.h>
#include "../Enemy.h"

void BossHp::Start()
{
	Enemy& boss = Get<Enemy>(objBoss);
	maxHP       = boss.Data().hp;
}

void BossHp::Update()
{
	Enemy& boss  = Get<Enemy>(objBoss);
	hp           = boss.Data().hp;
	text.content = "%d / %d", hp, maxHP;
}
