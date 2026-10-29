#include "pch\pch.h"
#include "PlayerHand.h"
#include "ItemSystem.h"

void PlayerHand::Start() {}

void PlayerHand::Update()
{
	ImGui::Begin("PlayerHand");

	ImGui::DragFloat3("offset", transform.position.v, 0.01f);

	ImGui::End();
}

void PlayerHand::Draw()
{
	Resource<Model> hModel = Get<ItemSystem>().GetModel(haveItemId);

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	// hModel.Get().Draw(Get<DeltaTime>() * animSpeed, animIndex);

	const auto& subParts{ hModel.Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}
