#include "pch\pch.h"
#include "EnemyController.h"
#include "Enemy.h"
#include "UI/BossInfo.h"
#include "UI/BossHp.h"
#include "UI/BossName.h"
#include "UI/BossGauge.h"
#include "UI/BossGaugeR.h"
#include "EnemyHpBar.h"
#include "PlaySceneUIController.h"

void EnemyController::Start() {}

void EnemyController::Update() {}

void EnemyController::AddEnemy(
    const GameObject _enemy)
{
	// 敵に固有IDを割り振る
	Enemy& enemy = Get<Enemy>(_enemy);

	enemy.SetID(nextEnemyId++);
	allEnemies.push_back(_enemy);

	// ID割り当て後にステートを登録
	NetworkActorServer& networkActor = Get<NetworkActorServer>(Data().objNetworkActorServer);

	networkActor.EnemyStateRegistries(_enemy);

	if (enemy.Data().isBoss)
	{
		PlaySceneUIController playSceneUIController = Get<PlaySceneUIController>(objPlaySceneUIController);
		Get<BossInfo>(playSceneUIController.Data().objBossInfo).SetBoss(_enemy);
		Get<BossInfo>(playSceneUIController.Data().objBossInfo).SetActive(true);
		Get<BossHp>(playSceneUIController.Data().objBossHp).SetActive(true);
		Get<BossName>(playSceneUIController.Data().objBossName).SetActive(true);
		Get<BossGauge>(playSceneUIController.Data().objBossGauge).SetActive(true);
		Get<BossGaugeR>(playSceneUIController.Data().objBossGaugeR).SetActive(true);
	}
	else
	{
		objEnemyHpBar.push_back(
		    GetScene().Instantiate<EnemyHpBar>(_enemy, enemy.GetPosition(), enemy.Data().hp, enemy.Data().hp));
	}
}

std::vector<Vec3> EnemyController::GetAllEnemyPosition()
{
	std::vector<Vec3> enemyPositions;

	for (const GameObject obj : allEnemies)
	{
		if (obj.IsEmpty())
		{
			continue;
		}

		enemyPositions.push_back(Get<Enemy>(obj).GetPosition());
	}

	return enemyPositions;
}

std::vector<float> EnemyController::GetAllEnemyAngleY()
{
	std::vector<float> enemyAngles;

	for (const GameObject obj : allEnemies)
	{
		if (obj.IsEmpty())
			continue;

		enemyAngles.push_back(Get<Enemy>(obj).GetAngleY());
	}

	return enemyAngles;
}

std::vector<int32_t> EnemyController::GetAllEnemyId()
{
	std::vector<int32_t> enemyIds;

	for (const GameObject obj : allEnemies)
	{
		if (obj.IsEmpty())
			continue;
		enemyIds.push_back(Get<Enemy>(obj).GetID());
	}

	return enemyIds;
}

std::vector<EnemyStatus> EnemyController::GetAllEnemyStatus()
{
	std::vector<EnemyStatus> allStatus;

	for (const GameObject obj : allEnemies)
	{
		if (obj.IsEmpty())
			continue;

		allStatus.push_back(Get<Enemy>(obj).GetStatus());
		EnemyStatus status = Get<Enemy>(obj).GetStatus();

		VCU_LOG("enemycontrollerdata: ID={}", status.enemyId);
	}
	VCU_LOG("送信する敵の数: {}", allStatus.size());

	return allStatus;
}

std::vector<GameObject> EnemyController::GetAllEnemies()
{
	return allEnemies;
}

void EnemyController::DamagedEnemy(
    int32_t enemyId,
    int32_t damage)
{
	for (const GameObject& enemyObj : allEnemies)
	{
		Enemy& enemy = Get<Enemy>(enemyObj);

		if (enemy.GetID() != enemyId)
		{
			continue;
		}

		enemy.Damaged(damage);
		return;
	}

	VCU_LOG("ダメージ対象の敵が見つかりません: ID={}", enemyId);
}

void EnemyController::RemoveEnemy(
    int32_t enemyId)
{
	for (GameObject& enemyObj : allEnemies)
	{
		if (enemyObj.IsEmpty())
		{
			continue;
		}

		Enemy& enemy = Get<Enemy>(enemyObj);

		if (enemy.GetID() != enemyId)
		{
			continue;
		}

		// 管理スロットだけを空にする
		enemyObj = {};

		return;
	}
}

bool EnemyController::IsExistEnemy(
    int32_t enemyId)
{
	for (GameObject& enemyObj : allEnemies)
	{
		if (enemyObj.IsEmpty())
		{
			continue;
		}

		if (Get<Enemy>(enemyObj).GetID() == enemyId)
		{
			return true;
		}
	}

	return false;
}

void EnemyController::SetNetworkActorServer(
    GameObject obj)
{
	Data().objNetworkActorServer = obj;
}

GameObject EnemyController::GetNetworkActorServer() const
{
	return Data().objNetworkActorServer;
}
