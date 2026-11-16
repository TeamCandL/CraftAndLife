#include "pch\pch.h"
#include "BossInfo.h"
#include "../Enemy.h"
#include "../../Manager/SessionManager/SessionStateReader.h"
#include <Game/Manager/AudioManager.h>

void BossInfo::Start()
{
	isBossActive = false;
}

void BossInfo::Update()
{
	if (isBossActive)
	{
		SessionManager& sessionManager = Get<SessionManager>();

		if (sessionManager.RefStatus().imLobbyHost)
		{
			Enemy& boss = Get<Enemy>(objBoss);
			hpInfo.hp   = boss.Data().hp;
		}
		else
		{
			// ゲスト側は同期されたHPを取得
			SessionStateReader reader{ sessionManager };

			int32_t hp = 0;

			if (reader.TryGetState(StateTarget::Enemy(bossEnemyId), "hp", hp))
			{
				hpInfo.hp = hp;
			}
		}

		hpInfo.rateOfHp = static_cast<float>(hpInfo.hp) / static_cast<float>(hpInfo.maxHP);

		if (hpInfo.hp <= 0)
		{
			isBossActive = false;

			// BOSS撃破時にBGMを停止
			Get<AudioManager>().SetBGMStop();
		}
	}
	else
	{
		hpInfo.hp       = 0;
		hpInfo.rateOfHp = 0.0f;
		SetActive(false);
	}
}

// ホスト用
void BossInfo::SetBoss(
    GameObject objBoss_)
{
	isBossActive = true;
	objBoss      = objBoss_;

	Enemy& boss = Get<Enemy>(objBoss);
	bossEnemyId = boss.GetID();

	hpInfo.maxHP    = boss.Data().hp;
	hpInfo.hp       = hpInfo.maxHP;
	hpInfo.rateOfHp = 1.0f;

	// BOSS戦BGMをループ再生
	Get<AudioManager>().SetBGM(bossBGM, true, 0.3f);
}

// ゲスト用
void BossInfo::SetBoss(
    int32_t enemyId,
    int32_t maxHP)
{
	isBossActive = true;
	bossEnemyId  = enemyId;

	hpInfo.maxHP    = maxHP;
	hpInfo.hp       = hpInfo.maxHP;
	hpInfo.rateOfHp = 1.0f;

	SetActive(true);

	// BOSS戦BGMをループ再生
	Get<AudioManager>().SetBGM(bossBGM, true);
}
