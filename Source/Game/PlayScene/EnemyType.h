#pragma once

/// @brief 敵の種類
enum EnemyType
{
	Skeleton = 0,
	BossSkeleton,
	Spider,
	Bat,
	Dragon,
	EnemyType_Max,
};

inline EnemyType ToEnemyType(
    int typeId)
{
	switch (typeId)
	{
	case 0:
		return EnemyType::Skeleton;
	case 1:
		return EnemyType::Spider;
	case 2:
		return EnemyType::Bat;
	case 3:
		return EnemyType::Dragon;
	default:
		throw std::invalid_argument("不正なEnemyType ID");
	}
}