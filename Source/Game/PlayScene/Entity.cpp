#include "Entity.h"
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/TerrainSystem.h>

void Entity::Start()
{
	model = Get<ItemSystem>().GetModel(itemId);
}

void Entity::Update()
{
	if (picked)
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
			Pick();  // デスポーンは神が拾った判定にしちゃう
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

	if (isHitCollider)
	{
		velocity *= std::pow(friction * 0.01f, DELTA_TIME);
	}
	else
	{
		velocity *= std::pow(friction, DELTA_TIME);
	}
}

void Entity::Draw()
{
	if (picked)
	{
		return;  // 取得されているなら無視
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

void Entity::Pick()
{
	picked = true;
}

bool Entity::CanPickup() const
{
	return pickupCooldownSec <= 0.0f && !picked;
}
