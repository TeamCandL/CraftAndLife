#include "pch\pch.h"
#include "BossName.h"
#include <Game/Manager/SessionManager.h>
#include "BossInfo.h"

void BossName::Start()
{
	SetActive(false);
}

void BossName::Update()
{
	SetActive(Get<BossInfo>(objBossInfo).IsBossActive());
	text.content = "BossName";
}
