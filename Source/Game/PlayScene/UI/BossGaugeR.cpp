#include "pch\pch.h"
#include "BossGaugeR.h"
#include "BossInfo.h"

void BossGaugeR::Start()
{
	SetActive(false);
}

void BossGaugeR::Update()
{
	SetActive(Get<BossInfo>(objBossInfo).IsActive());
}