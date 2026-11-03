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
	int32_t entityId{};
	ItemId itemId{};

	EntitySourceType sourceType{};
	int32_t sourceId{};

	Vec3 position{};
	Vec3 velocity{};
};

/// @brief エンティティ更新時の通信データ
struct EntityUpdateData
{
	int32_t entityId{};

	Vec3 position{};
	Vec3 velocity{};
};

/// @brief エンティティ削除時の通信データ
struct EntityRemoveData
{
	int32_t entityId{};
};