#pragma once
#include "ItemId.h"

/// @brief エンティティの生成元
enum class EntitySourceType
{
	Player,
	Enemy,
	Object,
};

/// @brief エンティティ生成時の通信データ
struct EntitySpawnData
{
	ItemId itemId{};
	Vec3 position{};
	Vec3 velocity{};
};
