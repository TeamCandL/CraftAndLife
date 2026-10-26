#include "Entity.h"
#include <Game/PlayScene/ItemSystem.h>

void Entity::Start()
{
	model = Get<ItemSystem>().GetModel(itemId);
}

void Entity::Update() {}

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
}
