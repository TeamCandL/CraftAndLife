#include "pch\pch.h"
#include "EntitySystem.h"
#include "IEntity.h"
#include "Entity.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "ItemSystem.h"
#include <Game/Manager/AudioManager.h>
#include <Game/Manager/SessionManager.h>

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

		if (Get<SessionManager>().RefStatus().imLobbyHost && entity.CanHitToHarvestable())
		{
			DynamicOBB* pDynamicOBB = TryGet<DynamicOBB>(obj);

			if (pDynamicOBB)
			{
				for (auto& info : pDynamicOBB->hitGameObjects)
				{
					Name* pName = TryGet<Name>(info.gameObject);

					if (pName && pName->content == "ハーベスタブル")
					{
						entity.SetUnderground(true);
						break;
					}
				}
			}
		}

		if (entity.IsThrowingItemEntity())
		{
			if (entity.IsUnderground())
			{
				// 地面の下にいる！

				if (Get<SessionManager>().RefStatus().imLobbyHost)
				{
					// ホストである
					// ならアイテム化しちゃう！
					ItemId collectibleItemid = Get<ItemSystem>().ConvertAttackItemToCollectibleItem(entity.GetItemId());

					if (collectibleItemid != 0)
					{
						Get<EntitySystem>().Spawn(collectibleItemid, entity.GetPosition(), Vec3::Zero());
					}
				}

				entity.Despawn();
				continue;
			}
		}

		if (entity.CanHitToPlayer())
		{
			DynamicOBB* pDynamicOBB = TryGet<DynamicOBB>(obj);

			if (pDynamicOBB)
			{
				for (auto& info : pDynamicOBB->hitGameObjects)
				{
					Name* pName = TryGet<Name>(info.gameObject);

					if (pName && pName->content == "Player")
					{
						entity.Pick();

						picksData.push_back(EntityPickData{
						    .entityId = entity.GetEntityId(),
						    .position = entity.GetPosition(),
						});
					}
				}
			}
		}

		if (entity.CanHitToEnemy())
		{
			DynamicOBB* pDynamicOBB = TryGet<DynamicOBB>(obj);

			if (pDynamicOBB)
			{
				for (auto& info : pDynamicOBB->hitGameObjects)
				{
					Name* pName = TryGet<Name>(info.gameObject);

					if (pName && pName->content == "Enemy")
					{
						entity.Pick();

						picksData.push_back(EntityPickData{
						    .entityId = entity.GetEntityId(),
						    .position = entity.GetPosition(),
						});

						Enemy& enemy = Get<Enemy>(info.gameObject);

						const ItemId itemId = entity.GetItemId();

						const int32_t damage = Get<ItemSystem>().GetInfo(itemId)._damageForEnemy;

						// 剣・大剣の攻撃だけ種類を判別して渡す
						if (itemId == 22)  // 通常の剣
						{
							enemy.Damaged(damage, false);
						}
						else if (itemId == 23)  // 大剣
						{
							enemy.Damaged(damage, true);
						}
						else
						{
							// 弓など、その他の攻撃は従来どおり
							enemy.Damaged(damage);
						}
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
			continue;
		}

		IEntity& entity = Get<IEntity>(obj);

		if (entity.CanPickup() == false)
		{
			continue;
		}

		Transform* pTransform = TryGet<Transform>(obj);

		if (pTransform)
		{
			const float DISTANCE = (pTransform->position - _pos).Length();

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

		entity.Pick();

		picksData.push_back(EntityPickData{
		    .entityId = entity.GetEntityId(),
		    .position = entity.GetPosition(),
		});
	}

	return objMinEntity;
}

GameObject EntitySystem::SpawnFromNetwork(
    const int32_t _entityId,
    const ItemId _itemId,
    const Vec3 _position,
    const Vec3 _velocity)
{
	objEntities.emplace_back(GetScene().Instantiate<Entity>(_itemId, _entityId));

	Get<IEntity>(objEntities.back()).SetPosition(_position);
	Get<IEntity>(objEntities.back()).SetVelocity(_velocity);
	return objEntities.back();
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
		return;
	}

	Get<IEntity>(objEntities[_entityId]).Pick();
	GetScene().Destroy(objEntities[_entityId]);
}

Vec3 EntitySystem::GetFacingTheEnemy(
    GameObject _objEnemyController,
    GameObject _objEntity)
{
	EnemyController& enemyController = Get<EnemyController>(_objEnemyController);
	Entity& entity                   = Get<Entity>(_objEntity);
	Vec3 Fwd                         = entity.GetForward();
	Vec3 targetPos{};
	float targetDist{ -1.0f };
	if (enemyController.Data().allEnemies.size() > 0)
	{
		for (GameObject& objEnemy : enemyController.GetAllEnemies())
		{
			Enemy& enemy = Get<Enemy>(objEnemy);

			float dist = (enemy.GetPosition() - entity.GetPosition()).Length();
			if (targetDist == -1.0f || dist < targetDist)
			{
				targetDist = dist;
				targetPos  = enemy.GetPosition();
			}
		}
		if (targetPos != entity.GetPosition())
		{
			Fwd = (targetPos - entity.GetPosition()).Normalize();
		}
	}

	return Fwd;
}