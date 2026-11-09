#include "pch\pch.h"
#include "EntitySystem.h"
#include "IEntity.h"
#include "Entity.h"

void EntitySystem::Update()
{
	for (auto& obj : objEntities)
	{
		if (obj.IsEmpty())
		{
			continue;  // オブジェクトがないなら無視
		}
		IEntity& entity = Get<IEntity>(obj);
		if (entity.IsPicked() || entity.IsDespawned())
		{
			GetScene().Destroy(obj);
			continue;
		}

		if (entity.CanHitToPlayer())
		{
			DynamicOBB* pDynamicOBB = TryGet<DynamicOBB>(obj);
			if (pDynamicOBB)
			{
				for (auto& info : pDynamicOBB->hitGameObjects)
				{
					///	取得したオブジェクトの名前を『.content』で取得。
					///	※ここでは、それがプレイヤーであるかを判定している。
					Name* pName = TryGet<Name>(info.gameObject);
					if (pName && pName->content == "Player")
					{
						entity.Pick();  // 拾いましたよ！(ダメージを)
						picksData.push_back(EntityPickData{
						    .entityId = entity.GetEntityId(),
						    .position = entity.GetPosition(),
						});
					}
				}
			}
		}
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
	float minDistance{ _radius };
	GameObject objMinEntity{ GameObject::Empty() };

	for (auto& obj : objEntities)
	{
		if (obj.IsEmpty())
		{
			continue;  // objが空っぽなら無視
		}

		IEntity& entity = Get<IEntity>(obj);

		if (entity.CanPickup() == false)
		{
			continue;  // 拾えません！無視
		}

		Transform* pTransform = TryGet<Transform>(obj);
		if (pTransform)
		{
			const float DISTANCE = (pTransform->position - _pos).Length();

			// 距離がより近いなら選ぶ
			if (DISTANCE <= minDistance)
			{
				minDistance  = DISTANCE;
				objMinEntity = obj;
			}
		}
	}

	if (!objMinEntity.IsEmpty())
	{
		IEntity& entity = Get<IEntity>(objMinEntity);
		entity.Pick();  // 拾いましたよ！
		picksData.push_back(EntityPickData{
		    .entityId = entity.GetEntityId(),
		    .position = entity.GetPosition(),
		});
	}

	return objMinEntity;
}

void EntitySystem::SpawnFromNetwork(
    const int32_t _entityId,
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	objEntities.emplace_back(GetScene().Instantiate<Entity>(_itemId, _entityId));
	Get<IEntity>(objEntities.back()).SetPosition(_position);
	Get<IEntity>(objEntities.back()).SetVelocity(_velocity);
}

void EntitySystem::SetHost(
    const bool _isHost)
{
}

void EntitySystem::RemoveFromNetwork(
    const int32_t _entityId)
{
	if (objEntities[_entityId].IsEmpty())
	{
		return;  // すでに消されているなら無視
	}
	Get<IEntity>(objEntities[_entityId]).Pick();
	GetScene().Destroy(objEntities[_entityId]);
}
