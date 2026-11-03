#include "pch\pch.h"
#include "BossGauge.h"
#include "BossHp.h"

void BossGauge::Start()
{
	BossHp& bossHp = Get<BossHp>(objBossHp);
	rateOfHp       = bossHp.GetRateOfHp();
}

void BossGauge::Update()
{
	if (Get<BossHp>(objBossHp).Data().isBossActive)
	{
		if (rateOfHp < 0.0f)
			rateOfHp = 0.0f;
		if (rateOfHp > 1.0f)
			rateOfHp = 1.0f;

		rateOfHp          = Get<BossHp>(objBossHp).GetRateOfHp();
		transform.scale.x = 547 * rateOfHp;
	}
	else
	{
		rateOfHp          = 0.0f;
		transform.scale.x = 0.0f;
	}
}
