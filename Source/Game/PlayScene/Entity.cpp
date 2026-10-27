#include "Entity.h"
#include <Game/PlayScene/ItemSystem.h>
#include <Game/PlayScene/TerrainSystem.h>

void Entity::Start()
{
	model = Get<ItemSystem>().GetModel(itemId);
}

void Entity::Update()
{
	bool isHitCollider{ false };

	const float DELTA_TIME = Get<DeltaTime>();

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
