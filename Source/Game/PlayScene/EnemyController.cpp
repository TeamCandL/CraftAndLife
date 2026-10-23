#include "pch\pch.h"
#include "EnemyController.h"
#include "Enemy.h"

void EnemyController::AddEnemy(
    const GameObject _enemy)
{
	allEnemys.push_back(_enemy);
}

std::vector<Vec3> EnemyController::GetAllEnemyPosition()
{
	std::vector<Vec3> enemyPositions;

	for (const GameObject obj : allEnemys)
	{
		enemyPositions.push_back(Get<Enemy>(obj).GetPosition());
	}

	return enemyPositions;
}

std::vector<float> EnemyController::GetAllEnemyAngleY()
{
	std::vector<float> enemyAngles;

	for (const GameObject obj : allEnemys)
	{
		enemyAngles.push_back(Get<Enemy>(obj).GetAngleY());
	}

	return enemyAngles;
}

std::vector<int32_t> EnemyController::GetAllEnemyId()
{
	std::vector<int32_t> enemyIds;

	for (const GameObject obj : allEnemys)
	{
		enemyIds.push_back(Get<Enemy>(obj).GetID());
	}

	return enemyIds;
}
