#include "pch\pch.h"
#include "NPCRobo.h"
#include <Game/PlayScene/TerrainSystem.h>
#include "AI/IAIReferenceDayController.h"
#include "AI/IAIReferenceEnemyController.h"
#include "AI/IAIReferenceHarvestableController.h"
#include "AI/IAIReferenceAllPlayers.h"
#include "AI/IAIReferencePlayer.h"
#include "../Gift/GiftRequestBox.h"
#include <gemilib/gemimath.h>
#include <shigaLibrary/SgRandom.h>

void NPCRobo::Start()
{
	transform.scale = Vec3::One() * 200.0f;

	objUIGiftRequestBox = GetScene().Instantiate<GiftRequestBox>();
}

void NPCRobo::Update()
{
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	Get<Transform>(objUIGiftRequestBox).position = transform.position + Vec3::Up() * 1.5f;
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
#pragma region 条件分岐
	auto condIsDaytime = std::make_shared<AINodeCondition>(
	    "今が昼であるか",
	    [this]
	    {
		    if (objDayController.IsEmpty())
		    {
			    return false;
		    }
		    return Get<IAIReferenceDayController>(objDayController).IsDaytime();
	    });

	auto condIsTargetNearby = std::make_shared<AINodeCondition>(
	    "ターゲットが近いか",
	    [this]
	    {
		    if (objTarget.IsEmpty())
		    {
			    return false;
		    }
		    return (Get<Transform>(objTarget).WorldPosition() - transform.position).Length() <=
		           AI_CONFIG.NEARBY_DISTANCE;
	    });

	auto condIsLowHp = std::make_shared<AINodeCondition>(
	    "自身の体力が低いか",
	    [this]
	    {
		    return hp <= AI_CONFIG.LOW_HP_VALUE;
	    });

	auto condCanPerceiveTarget = std::make_shared<AINodeCondition>(
	    "ターゲットを知覚できるか",
	    [this]
	    {
		    if (objTarget.IsEmpty())
		    {
			    return false;
		    }
		    return GemiMath::IsAngleWithinRadiansByCross3D(
		        (Get<Transform>(objTarget).WorldPosition() - transform.position).Normalize(),
		        transform.Forward(),
		        DirectX::XMConvertToRadians(AI_CONFIG.PERCEIVE_DEG));
	    });
	auto condElse = std::make_shared<AINodeCondition>(
	    "else",
	    [this]
	    {
		    return true;
	    });
#pragma endregion

#pragma region 実行
	auto actSetTargetNearbyHarvestable = std::make_shared<AINodeAction>(
	    "近くのハーベスタブルオブジェクトをターゲットにする",
	    [this]
	    {
		    objTarget = Get<IAIReferenceHarvestableController>(objHarvestableController)
		                    .GetNearbyHarvestableObject(transform.position);
		    return AIResult_Yes;
	    });

	auto actSetTargetNearbyPlayer = std::make_shared<AINodeAction>(
	    "近くのプレイヤーをターゲットにする",
	    [this]
	    {
		    objTarget = Get<IAIReferenceAllPlayers>(objAllPlayers).GetNearbyPlayerObject(transform.position);
		    return AIResult_Yes;
	    });

	auto actSetTargetNearbyEnemy = std::make_shared<AINodeAction>(
	    "近くの敵をターゲットにする",
	    [this]
	    {
		    objTarget = Get<IAIReferenceEnemyController>(objEnemyController).GetNearbyEnemyObject(transform.position);
		    return AIResult_Yes;
	    });

	auto actSetPositionMoveToRandom = std::make_shared<AINodeAction>(
	    "ランダムな場所を移動先にする",
	    [this]
	    {
		    posMoveTo   = Vec3{ Random::GetRandom(AI_CONFIG.MAP_POSITION_MAX.x, AI_CONFIG.MAP_POSITION_MIN.x),
                              0,
                              Random::GetRandom(AI_CONFIG.MAP_POSITION_MAX.y, AI_CONFIG.MAP_POSITION_MIN.y) };
		    posMoveTo.y = Get<TerrainSystem>().GetHeight(posMoveTo.x, posMoveTo.z);
		    return AIResult_Yes;
	    });

	auto actSetPositionMoveToTarget = std::make_shared<AINodeAction>(
	    "ターゲットを移動先にする",
	    [this]
	    {
		    if (objTarget.IsEmpty())
		    {
			    return AIResult_No;  // スキップする
		    }
		    posMoveTo = Get<Transform>(objTarget).WorldPosition();
		    return AIResult_Yes;
	    });

	auto actSetWantEmpty = std::make_shared<AINodeAction>(
	    "欲しいアイテムを空にする",
	    [this]
	    {
		    itemIdWant = EMPTY_ITEM_ID;
		    return AIResult_Yes;
	    });

	auto actSetWantWood = std::make_shared<AINodeAction>(
	    "欲しいアイテムを木材にする",
	    [this]
	    {
		    itemIdWant = AI_CONFIG.ITEM_ID_WOOD;
		    return AIResult_Yes;
	    });

	auto actSetWantRock = std::make_shared<AINodeAction>(
	    "欲しいアイテムを石にする",
	    [this]
	    {
		    itemIdWant = AI_CONFIG.ITEM_ID_ROCK;
		    return AIResult_Yes;
	    });

	auto actSetWantRandom = std::make_shared<AINodeAction>(
	    "欲しいアイテムをランダムに指定する",
	    [this]
	    {
		    itemIdWant = Random::GetRandom(AI_CONFIG.ITEM_ID_MIN, AI_CONFIG.ITEM_ID_MAX);
		    return AIResult_Yes;
	    });

	auto actLookAtMovePos = std::make_shared<AINodeAction>(
	    "移動先へ向く",
	    [this]
	    {
		    if (objTarget.IsEmpty())
		    {
			    return AIResult_No;
		    }
		    Vec3 dir           = (Get<Transform>(objTarget).position - transform.position).Normalize();
		    transform.rotation = Quaternion::FromEuler(Vec3::Up() * std::atan2f(dir.z, dir.x));
		    return AIResult_Yes;
	    });

	auto actMoveToPos = std::make_shared<AINodeAction>(
	    "移動先に進む",
	    [this]
	    {
		    transform.position += transform.Forward() * AI_CONFIG.MOVE_SPEED_PER_SEC * Get<DeltaTime>();
		    return AIResult_Yes;
	    });
#pragma endregion

	/*rootSequenceNode.nodes.emplace_back(std::shared_ptr<AINodeSwitch>(
	    "",
	    std::vector<std::pair<std::shared_ptr<AINodeCondition>, std::shared_ptr<AINode>>>{
	        { condIsLowHp, std::make_shared<AINodeSequence>() } }));*/
}

void NPCRobo::UpdateAI() {}
