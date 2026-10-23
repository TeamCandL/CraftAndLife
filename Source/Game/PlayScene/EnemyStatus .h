#pragma once
#include "EnemyType.h"
#include <Game/Game.h>

/// 敵のID,敵の種類,座標,回転,を入れる構造体
struct EnemyStatus
{
	int enemyId;
	EnemyType enemyType;
	Transform enemyPosition;
	float enemyAngleY;
};
