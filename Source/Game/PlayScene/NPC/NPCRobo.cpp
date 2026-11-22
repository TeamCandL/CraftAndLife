#include "pch\pch.h"
#include "NPCRobo.h"
#include <Game/PlayScene/TerrainSystem.h>
#include "AI/IAIReferenceDayController.h"
#include "AI/IAIReferenceEnemyController.h"
#include "AI/IAIReferenceHarvestableController.h"
#include "AI/IAIReferencePlayer.h"

void NPCRobo::Start()
{
	transform.scale = Vec3::One() * 200.0f;
}

void NPCRobo::Update()
{
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);
}

void NPCRobo::Draw()
{
	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

#ifdef _DEBUG
	ImGui::Begin("NPC-Draw");
	float animTime = model.Get().currentAnimTime;
	ImGui::DragFloat("currentAnimTime", &animTime, 0.0001f);
	ImGui::DragFloat("playSpeed", &animPlaySpeed, 0.0001f);
	model.Get().SetCurrentAnimTime(animTime);

	ImGui::DragInt("animIndex", &animIndex);
	ImGui::End();
#endif
	// Get<DeltaTime>() * animSpeed * 0.0001f
	model.Get().Draw(Get<DeltaTime>() * animPlaySpeed, animIndex);

	const auto& subParts{ model.Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void NPCRobo::InitializeAI()
{
	/*auto isDaytime = std::make_shared<AINodeCondition>(
	    []
	    {
	        return
	    });*/
}

void NPCRobo::UpdateAI() {}
