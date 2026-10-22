#include "EnemyAiCheckPoint.h"
#include "pch\pch.h"

void EnemyCheckPoint::Start()
{
	obb.offset = OBB_OFFSET;
	obb.scale  = Vec3::One() * OBB_SCALE;
}

void EnemyCheckPoint::Update() {}
