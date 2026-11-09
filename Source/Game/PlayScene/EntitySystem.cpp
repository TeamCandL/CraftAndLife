#include "EntitySystem.h"
#include "Entity.h"
#include "IEntity.h"
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/NetworkActorServer.h>

void EntitySystem::Start()
{
	VCU_LOG("Start");
}

void EntitySystem::Update()
{
	const float DELTA_TIME{ Get<DeltaTime>() };

	std::vector<int32_t> justSpawnedIds;  // 生成直後のエンティティIDを記録するためのベクター

	for (auto& info : entities)
	{
		info.pickupCooldownSec -= DELTA_TIME;

		if (info.isJustSpawned)
		{
			info.isJustSpawned = false;
			continue;
		}
	}

	// 自然消滅タイマーはホストだけが管理
	if (Data().isHost)
	{
		for (auto& info : entities)
		{
			if (info.isJustSpawned)
			{
				continue;
			}

			info.despawnTimeLeft -= DELTA_TIME;
		}

		std::vector<int32_t> removeIds;

		for (const auto& info : entities)
		{
			if (!info.isJustSpawned && !info.isRemoveRequested && info.despawnTimeLeft <= 0.0f)
			{
				removeIds.push_back(info.entityId);
			}
		}

		for (int32_t entityId : removeIds)
		{
			Remove(entityId);
		}
	}

	// Updateの最後に生成直後フラグを解除
	for (auto& info : entities)
	{
		if (std::find(justSpawnedIds.begin(), justSpawnedIds.end(), info.entityId) != justSpawnedIds.end())
		{
			info.isJustSpawned = false;
		}
	}
}

void EntitySystem::Spawn(
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	ItemInfo info = Get<ItemSystem>().GetInfo(_itemId);

	// info.
	// TODO: 自然消滅とかの情報ほしい

	const int32_t entityId = Data().nextEntityId++;

	// entities.emplace_back(GetScene().Instantiate<Entity>(_itemId), 3.0f, 60.0f * 5.0f);
	// IEntity& entity = Get<IEntity>(entities.back().obj);

	// TODO: 自然消滅タイムの初期化

	// entity.SetPosition(_position);
	// entity.SetVelocity(_velocity);

	// Data().entities.back().entityId = entityId;

	EntitySpawnData spawnData{};

	spawnData.entityId = entityId;
	spawnData.itemId   = _itemId;
	spawnData.position = _position;
	spawnData.velocity = _velocity;

	// 生成元は呼び出し元に応じて設定する
	spawnData.sourceType = EntitySourceType::Object;
	spawnData.sourceId   = 0;

	Data().pendingSpawnData.push_back(spawnData);
}

GameObject EntitySystem::PickNearest(
    const Vec3 _pos,
    const float _radius)
{
	float minDistance{ _radius };
	EntityInfo* pMinEntityInfo{};

	for (auto& info : entities)
	{
		if (info.pickupCooldownSec > 0.0f)
		{
			continue;  // 拾えないなら無視
		}

		if (info.isRemoveRequested)
		{
			continue;  // 削除要求済みの場合は無視
		}

		Transform* pTransform = TryGet<Transform>(info.obj);

		if (!pTransform)
		{
			continue;  // transformがないなら無視
		}

		const Vec3 entityPosition = pTransform->position;
		const float distance      = (entityPosition - _pos).Length();

		if (distance <= minDistance)
		{
			minDistance    = distance;
			pMinEntityInfo = &info;
		}
	}

	if (!pMinEntityInfo)
	{
		return GameObject{};
	}

	pMinEntityInfo->despawnTimeLeft = 0.0f;

	return pMinEntityInfo->obj;
}

void EntitySystem::SpawnFromNetwork(
    const int32_t _entityId,
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	// 同じIDのエンティティが既に存在する場合は生成しない
	for (const auto& info : entities)
	{
		if (info.entityId == _entityId)
		{
			return;
		}
	}

	// エンティティを生成
	// entities.emplace_back(GetScene().Instantiate<Entity>(_itemId), 60.0f * 5.0f, 3.0f);

	EntitySystemData::EntityInfo info{};
	info.obj               = GetScene().Instantiate<Entity>(_itemId, 0);
	info.pickupCooldownSec = 3.0f;
	info.despawnTimeLeft   = 60.0f * 5.0f;
	info.entityId          = _entityId;
	info.isJustSpawned     = true;

	IEntity& entity = Get<IEntity>(info.obj);

	// entities.push_back(std::move(info));
	// EntityInfo& info = entities.back();

	entity.SetPosition(_position);
	entity.SetVelocity(_velocity);

	entities.push_back(std::move(info));
}

const std::vector<EntitySpawnData>& EntitySystem::GetPendingSpawnData() const
{
	return Data().pendingSpawnData;
}

void EntitySystem::ClearPendingSpawnData()
{
	Data().pendingSpawnData.clear();
}

const std::vector<EntityRemoveData>& EntitySystem::GetPendingRemoveData() const
{
	return Data().pendingRemoveData;
}

void EntitySystem::ClearPendingRemoveData()
{
	Data().pendingRemoveData.clear();
}

void EntitySystem::Remove(
    int32_t _entityId)
{
	for (auto& info : entities)
	{
		if (info.entityId != _entityId)
		{
			continue;
		}

		// 既に削除要求を登録している場合は何もしない
		if (info.isRemoveRequested)
		{
			return;
		}

		// 削除要求済みにする
		info.isRemoveRequested = true;

		EntityRemoveData removeData{};
		removeData.entityId = _entityId;

		// 削除要求を登録
		Data().pendingRemoveData.push_back(removeData);

		return;
	}
}

void EntitySystem::RemoveFromNetwork(
    int32_t _entityId)
{
	for (auto itr = entities.begin(); itr != entities.end(); ++itr)
	{
		if (itr->entityId != _entityId)
		{
			continue;
		}

		GetScene().Destroy(itr->obj);
		entities.erase(itr);

		return;
	}
}

void EntitySystem::UpdateEntityId(
    int32_t _oldId,
    int32_t _newId)
{
	for (auto& info : entities)
	{
		if (info.entityId != _oldId)
		{
			continue;
		}

		info.entityId = _newId;

		VCU_LOG("エンティティID更新: {} -> {}", _oldId, _newId);

		return;
	}

	VCU_LOG("ID更新対象のエンティティが見つからない: {}", _oldId);
}

void EntitySystem::SetHost(
    bool _isHost)
{
	Data().isHost = _isHost;
}
