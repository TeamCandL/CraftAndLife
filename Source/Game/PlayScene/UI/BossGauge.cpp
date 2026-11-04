#include "pch\pch.h"
#include "BossGauge.h"
#include "BossInfo.h"

void BossGauge::Start()
{
	BossInfo& bossInfo = Get<BossInfo>(objBossInfo);
	rateOfHp           = bossInfo.GetHpInfo().rateOfHp;

	SetActive(false);
}

void BossGauge::Update()
{
	SetActive(Get<BossInfo>(objBossInfo).IsBossActive());
	if (Get<BossInfo>(objBossInfo).IsBossActive())
	{
		if (rateOfHp < 0.0f)
			rateOfHp = 0.0f;
		if (rateOfHp > 1.0f)
			rateOfHp = 1.0f;

		rateOfHp          = Get<BossInfo>(objBossInfo).GetHpInfo().rateOfHp;
		transform.scale.x = 547 * rateOfHp;
	}
	else
	{
		rateOfHp          = 0.0f;
		transform.scale.x = 0.0f;
	}
}
