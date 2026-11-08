#include "pch\pch.h"
#include "EntitySystem.h"
#include "IEntity.h"
#include "Entity.h"

void EntitySystem::Update()
{
	for (auto& obj : objEntities)
	{
		Get<IEntity>(obj).
	}
}

void EntitySystem::Spawn(
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	// スポーンしたとの情報だけ
	spawnsData.emplace_back(_itemId, _position, _velocity);
}

GameObject EntitySystem::PickNearest(
    const Vec3 _pos,
    const float _radius)
{
	return GameObject();
}

void EntitySystem::SpawnFromNetwork(
    const int32_t _entityId,
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	objEntities.emplace_back(GetScene().Instantiate<Entity>(_itemId, _entityId));
}

void EntitySystem::SetHost(
    const bool _isHost)
{
}

void EntitySystem::RemoveFromNetwork(
    const int32_t _entityId)
{
}
