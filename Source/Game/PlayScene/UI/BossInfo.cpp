#include "pch\pch.h"
#include "BossInfo.h"
#include "../Enemy.h"

void BossInfo::Start()
{
	isBossActive = false;
}

void BossInfo::Update()
{

	if (isBossActive)
	{
		Enemy& boss     = Get<Enemy>(objBoss);
		hpInfo.hp       = boss.Data().hp;
		hpInfo.rateOfHp = static_cast<float>(hpInfo.hp) / static_cast<float>(hpInfo.maxHP);

		if (hpInfo.hp <= 0)
		{
			isBossActive = false;
		}
	}
	else
	{
		hpInfo.hp       = 0;
		hpInfo.rateOfHp = 0.0f;
		SetActive(false);
	}
}

void BossInfo::SetBoss(
    GameObject objBoss_)
{
	isBossActive    = true;
	objBoss         = objBoss_;
	hpInfo.maxHP    = Get<Enemy>(objBoss).Data().hp;
	hpInfo.hp       = hpInfo.maxHP;
	hpInfo.rateOfHp = 1.0f;
}
