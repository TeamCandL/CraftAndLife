#include "EnemyAiCheckPoint.h"
#include "pch\pch.h"

void EnemyCheckPoint::Start()
{
	obb.offset = OBB_OFFSET;
	obb.scale  = Vec3::One() * OBB_SCALE;

	obb.isTrigger = true;  ///< 当たり判定を貫通させる(押し出さなくなる)
}

void EnemyCheckPoint::Update() {}
