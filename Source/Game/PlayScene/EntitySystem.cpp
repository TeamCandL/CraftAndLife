#include "EntitySystem.h"
#include "Entity.h"
#include "IEntity.h"

void EntitySystem::Start() {}

void EntitySystem::Update() {}

void EntitySystem::Spawn(
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	entities.emplace_back(GetScene().Instantiate<Entity>(_itemId));
	IEntity& entity = Get<IEntity>(entities.back());

	entity.SetPosition(_position);
	entity.SetVelocity(_velocity);
}

GameObject EntitySystem::PickNearest(
    Vec3 _pos,
    float _radius)
{
	return GameObject();
}
