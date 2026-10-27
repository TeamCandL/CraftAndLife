#include "EntitySystem.h"
#include "Entity.h"
#include "IEntity.h"
#include <Game/PlayScene/ItemSystem.h>

void EntitySystem::Start()
{
	VCU_LOG("Start");
}

void EntitySystem::Update()
{
	const float DELTA_TIME{ Get<DeltaTime>() };

	for (size_t i = 0; i < entities.size(); i++)
	{
		auto& info = entities[i];
		info.despawnTimeLeft -= DELTA_TIME;
		info.pickupCooldownSec -= DELTA_TIME;
	}
	// return;
	for (auto itr = entities.begin(); itr != entities.end();)
	{
		if (itr->despawnTimeLeft <= 0.0f)
		{
			GetScene().Destroy(itr->obj);
			itr = entities.erase(itr);
		}
		else
		{
			itr++;
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

	entities.emplace_back(GetScene().Instantiate<Entity>(_itemId), 3.0f, 60.0f * 5.0f);
	IEntity& entity = Get<IEntity>(entities.back().obj);

	// TODO: 自然消滅タイムの初期化

	entity.SetPosition(_position);
	entity.SetVelocity(_velocity);
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

		Transform* pTransform = TryGet<Transform>(info.obj);

		if (!pTransform)
		{
			continue;  // transformがないなら無視
		}

		Vec3 entityPosition = pTransform->position;

		if ((entityPosition - _pos).Length() <= minDistance)
		{
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
