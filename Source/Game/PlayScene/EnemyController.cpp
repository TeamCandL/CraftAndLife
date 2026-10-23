#include "pch\pch.h"
#include "EnemyController.h"
#include "Enemy.h"

void EnemyController::AddEnemy(
    const GameObject _enemy)
{
	allEnemys.push_back(Get<Enemy>(_enemy));
}

std::vector<Vec3> EnemyController::GetAllEnemyPosition() const
{
	std::vector<Vec3> enemyPositions;

	for (const Enemy& enemy : allEnemys)
	{
		enemyPositions.push_back(enemy.GetPosition());
	}

	return enemyPositions;
}

std::vector<float> EnemyController::GetAllEnemyAngleY() const
{
	std::vector<float> enemyAngles;

	for (const Enemy& enemy : allEnemys)
	{
		enemyAngles.push_back(enemy.GetAngleY());
	}

	return enemyAngles;
}

std::vector<int32_t> EnemyController::GetAllEnemyId() const
{
	std::vector<int32_t> enemyIds;

	for (const Enemy& enemy : allEnemys)
	{
		enemyIds.push_back(enemy.GetID());
	}

	return enemyIds;
}
