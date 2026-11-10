#include "Entity.h"
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/TerrainSystem.h>

void Entity::Start()
{
	model = Get<ItemSystem>().GetModel(itemId);

	const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(itemId);
	if (ITEM_INFO._type == ItemType::EnemyAttack)
	{
		// 敵の攻撃なら落下しないし減速もしない！
		gravityPerSec          = 0.0f;
		friction               = 0.0f;
		isDestroyedOnCollision = true;          // 何かに触れたら消滅します
		canHitToPlayer         = true;          // プレイヤーへ触れたら取れます
		pickupCooldownSec      = 99999999.0f;   // 絶対に取得できない
		if (ITEM_INFO._coolTime > FLT_EPSILON)  // クールタイムが0より大きいなら
		{
			despawnTimeLeftSec = ITEM_INFO._coolTime;
		}
	}
	else if (ITEM_INFO._type == ItemType::Arrow)
	{
		canHitToEnemy = true;
	}
}

void Entity::Update()
{
	if (picked || despawned)
	{
		return;  // 取得されているなら無視
	}

	bool isHitCollider{ false };

	const float DELTA_TIME = Get<DeltaTime>();

	// ピックアップクールタイム更新！
	if (pickupCooldownSec > 0.0f)
	{
		pickupCooldownSec -= DELTA_TIME;
		if (pickupCooldownSec < 0.0f)
		{
			pickupCooldownSec = 0.0f;
		}
	}

	// デスポーンまでの時間更新！
	if (despawnTimeLeftSec > 0.0f)
	{
		despawnTimeLeftSec -= DELTA_TIME;
		if (despawnTimeLeftSec < 0.0f)
		{
			despawnTimeLeftSec = 0.0f;
			Despawn();  // デスポーンさせる サーバーへの送信はしない
		}
	}

	// 移動量y
	float deltaY = velocity.y * DELTA_TIME;

	const float HEIGHT = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	if (HEIGHT >= transform.position.y + deltaY)
	{
		velocity.y           = 0.0f;
		transform.position.y = HEIGHT;
		isHitCollider        = true;
	}
	else
	{
		// 落下していく
		velocity.y -= gravityPerSec * DELTA_TIME;
	}

	transform.position = transform.position + velocity * DELTA_TIME;

	if (isDestroyedOnCollision)
	{
		transform.position.y = HEIGHT;
	}

	// TODO: isDestroyedOnCollisionこれうそ
	if (isDestroyedOnCollision)
	{
		// Despawn();  // 地面に触れて自然消滅
		return;
	}
	else
	{
		if (isHitCollider)
		{
			velocity *= std::pow(friction * 0.01f, DELTA_TIME);
		}
		else
		{
			velocity *= std::pow(friction, DELTA_TIME);
		}
	}
}

void Entity::Draw()
{
	if (picked || despawned)
	{
		return;  // 取得されているなら無視
	}

	Get<Dx11Manager>().EnqueueTransparent(
	    [this]()
	    {
		    Get<Dx11Manager>().SetShader(mainShader);
		    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());
		    if (model.IsNull())
		    {
			    return;
		    }
		    Get<Dx11Manager>().SetShader(mainShader);
		    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

		    const auto& subParts{ model.Get().subParts };
		    for (const auto& parts : subParts)
		    {
			    Get<Dx11Manager>().SetSurface(parts.mesh);
			    Get<Dx11Manager>().SetMaterial(parts.material);
			    Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
			    Get<Dx11Manager>().Draw();
		    }

		    Get<Dx11Manager>().SetShader(edgeShader);
		    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

		    const auto& edgeParts{ model.Get().subParts };
		    for (const auto& parts : edgeParts)
		    {
			    Get<Dx11Manager>().SetSurface(parts.mesh);
			    Get<Dx11Manager>().SetMaterial(parts.material);
			    Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
			    Get<Dx11Manager>().Draw();
		    }

		    /*Get<Dx11Manager>().SetShader(edgeShader);
		    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

		    const auto& subParts{ model.Get().subParts };
		    for (const auto& parts : subParts)
		    {
		        Get<Dx11Manager>().SetSurface(parts.mesh);
		        Get<Dx11Manager>().SetMaterial(parts.material);
		        Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		        Get<Dx11Manager>().Draw();
		    }*/
	    });
}

void Entity::SetVelocity(
    const Vec3 _velocity)
{
	velocity = _velocity;
	if (velocity.Length() <= 0.1f)
	{
		// 速度が0なら方向はいじらない
	}
	else
	{
		transform.rotation = Quaternion::FromLookAtQuaternion(Vec3::Zero(), _velocity, Vec3::Up());
	}
}

bool Entity::IsPicked() const
{
	return picked;
}

bool Entity::IsDespawned() const
{
	return despawned;
}

void Entity::Despawn()
{
	despawned = true;
}

void Entity::Pick()
{
	picked = true;
}

bool Entity::CanPickup() const
{
	return pickupCooldownSec <= 0.0f && !picked && !despawned;
}

bool Entity::CanHitToPlayer() const
{
	return canHitToPlayer;
}

bool Entity::CanHitToEnemy() const
{
	return canHitToEnemy;
}

Vec3 Entity::GetPosition() const
{
	return transform.position;
}
