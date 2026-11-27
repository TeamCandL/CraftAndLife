#include "Entity.h"
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/TerrainSystem.h>
#include "EnemyController.h"
#include "Enemy.h"
#include "EntitySystem.h"
#include "../../../Library/shigaLibrary/SgMath.h"
#include <Game/Manager/AudioManager.h>

void Entity::Start()
{
	if (itemId == ITEM_ID_PLAYER_ATTACK_FIRE_BALL)
	{
		// 火炎攻撃SE
		Get<AudioManager>().Play(flameSE);
	}

	model     = Get<ItemSystem>().GetModel(itemId);
	obb.scale = Get<ItemSystem>().GetInfo(itemId).colliderSize;

	const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(itemId);
	if (ITEM_INFO._type == ItemType::EnemyAttack || ITEM_INFO._type == ItemType::PlayerAttackSword)
	{
		// 落下しないし減速もしない！
		gravityPerSec = 0.0f;
		friction      = 0.0f;
	}
	if (ITEM_INFO._type == ItemType::EnemyAttack)
	{
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
	else if (
	    ITEM_INFO._type == ItemType::PlayerAttackSword || ITEM_INFO._type == ItemType::PlayerAttackArrow ||
	    ITEM_INFO._id == ITEM_ID_PLAYER_ATTACK_OTHER)
	{
		pickupCooldownSec = 99999999.0f;  // 絶対に取得できない
		canHitToEnemy     = true;
		if (ITEM_INFO._coolTime > FLT_EPSILON)  // クールタイムが0より大きいなら
		{
			despawnTimeLeftSec = ITEM_INFO._coolTime;
		}
	}
	EntitySystem& entitySystem{ Get<EntitySystem>() };
	Vec3 fwdToEnemy{};
	switch (itemId)
	{
	case ITEM_ID_PLAYER_ATTACK_SWORD:
	case ITEM_ID_PLAYER_ATTACK_BIG_SWORD:
		fwdToEnemy = entitySystem.GetFacingTheEnemy(entitySystem.Data().objEnemyController, ToGameObject());
		SetForward(fwdToEnemy);
		break;
	case ITEM_ID_PLAYER_ATTACK_ARROW:
		isThrowingItemEntity = true;  // 投げ物
		break;
	case ITEM_ID_ENEMY_ATTACK_STRING:
		canHitToHarvestable  = true;
		isThrowingItemEntity = true;  // 投げ物
		break;
	default:
		break;
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
		if (isThrowingItemEntity && itemId != ITEM_ID_ENEMY_ATTACK_STRING)
		{
			isUnderground = true;
		}
		else
		{
			velocity.y           = 0.0f;
			transform.position.y = HEIGHT;
			isHitCollider        = true;
		}
	}
	else
	{
		// 落下していく
		velocity.y -= gravityPerSec * DELTA_TIME;
	}

	transform.position = transform.position + velocity * DELTA_TIME;

	if (isDestroyedOnCollision)
	{
		transform.position.y = HEIGHT + 1.0f;
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

	if (enemyDetected)
	{
		Vec3 move{ transform.Forward() * SPPED_OF_TRACKING_ENEMIES };
		transform.position += move * Get<DeltaTime>();
	}
}

void Entity::Draw()
{
	if (picked || despawned)
	{
		return;  // 取得されているなら無視
	}
	if (model.IsNull())
	{
		return;
	}
	Get<Dx11Manager>().EnqueueTransparent(
	    [this]()
	    {
		    Get<Dx11Manager>().SetShader(edgeShader);
		    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());
		    Get<Dx11Manager>().ChangeDepthState(DepthStateType_Background);

		    const auto& edgeParts{ model.Get().subParts };
		    for (const auto& parts : edgeParts)
		    {
			    Get<Dx11Manager>().SetSurface(parts.mesh);
			    Get<Dx11Manager>().SetMaterial(parts.material);
			    Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
			    Get<Dx11Manager>().Draw();
		    }

		    if (itemId != ITEM_ID_ENEMY_ATTACK_STRING)
		    {
			    Get<Dx11Manager>().ChangeDepthState(DepthStateType_Standard);

			    Get<Dx11Manager>().SetShader(mainShader);
			    Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

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
		    }
	    });
}

void Entity::DrawShadowMap()
{
	if (picked || despawned)
	{
		return;  // 取得されているなら無視
	}
	if (model.IsNull())
	{
		return;
	}

	Get<Dx11Manager>().SetShader(shadowShader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	const auto& subParts{ model.Get().subParts };
	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
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

bool Entity::IsThrowingItemEntity() const
{
	return isThrowingItemEntity;
}

bool Entity::IsUnderground() const
{
	return isUnderground;
}

void Entity::SetForward(
    Vec3 _Fwd)
{
	if (_Fwd.Length() <= FLT_EPSILON)
		return;

	transform.rotation = Quaternion::FromLookAtQuaternion(transform.position, transform.position + _Fwd, Vec3::Up());
	// Vec3 difference    = _Fwd - transform.Forward().Normalize();
	// const float RotationRatio = 1.0f;
	// float torque              = difference.y * RotationRatio;
	// Vec3 angles               = transform.rotation.ToEuler();
	// angles.x                  = 0.0f;
	// angles.y                  = torque;
	// angles.z                  = 0.0f;
	// transform.rotation        = transform.rotation + Quaternion::FromEuler(angles);
	enemyDetected = true;
}
