#pragma once
#include "EnemyType.h"
#include <Game/Game.h>

/// @brief 敵のステータス一覧 id, type, pos, angle
struct EnemyStatus
{
	int enemyId;
	EnemyType enemyType;
	Vec3 enemyPosition;
	float enemyAngleY;
};
