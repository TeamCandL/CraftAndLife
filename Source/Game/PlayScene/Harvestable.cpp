#include "pch/pch.h"
#include "Harvestable.h"
#include "EntitySystem.h"
#include <Game/Manager/AudioManager.h>

/// @brief 初期化
void Harvestable::Start()
{
	// 種類に応じたモデルと耐久値を設定
	switch (type)
	{
	case HarvestableType::Tree:
		model         = Resource<Model>{ "Model/Harvestable/Tree.fbx" };
		maxDurability = 60;
		obb.scale     = { 0.5f, 2.0f, 0.2f };
		break;

	case HarvestableType::TreeHigh:
		model         = Resource<Model>{ "Model/Harvestable/TreeHigh.fbx" };
		maxDurability = 120;
		obb.scale     = { 0.5f, 3.0f, 0.2f };
		break;

	case HarvestableType::Rock:
		model         = Resource<Model>{ "Model/Harvestable/Rock.fbx" };
		maxDurability = 100;
		obb.scale     = { 0.5f, 0.5f, 0.5f };
		break;

	default:
		// 未定義の種類は通常の木にする
		model         = Resource<Model>{ "Model/Harvestable/Tree.fbx" };
		maxDurability = 30;
		break;
	}

	// 耐久値を初期値に戻す
	durability = maxDurability;
}

/// @brief 更新
void Harvestable::Update()
{
	// 採取済みなら処理しない
	if (isHarvested)
	{
		return;
	}
}

/// @brief 描画
void Harvestable::Draw()
{
	// 採取済みなら描画しない
	if (isHarvested)
	{
		return;
	}

	// シェーダーを設定
	Get<Dx11Manager>().SetShader(shader);

	// ワールド変換情報を設定
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	// モデルを描画
	model.Get().Draw(Get<DeltaTime>(), animIndex);

	// モデルの各パーツを描画
	const auto& subParts{ model.Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

/// @brief ダメージを受ける
void Harvestable::TakeDamage(
    int32_t damage)
{
	// 採取済みなら処理しない
	if (isHarvested)
	{
		return;
	}

	// 0以下のダメージは無視する
	if (damage <= 0)
	{
		return;
	}

	// 耐久値を減らす
	durability -= damage;

	// 種類に応じた破壊音を再生
	switch (type)
	{
	case HarvestableType::Tree:
	case HarvestableType::TreeHigh:
		Get<AudioManager>().Play(treeDestructionSE);
		break;

	case HarvestableType::Rock:
		Get<AudioManager>().Play(rockDestructionSE);
		break;

	default:
		break;
	}

	// 耐久値が0以下になったら採取済みにする
	if (durability <= 0)
	{
		durability  = 0;
		isHarvested = true;
		OnBroken();
	}
}

/// @brief 採取済みかどうかを取得する
bool Harvestable::IsHarvested() const
{
	return isHarvested;
}

void Harvestable::Break()
{
	DestroyMe();
}

void Harvestable::OnBroken()
{
	switch (type)
	{
	case HarvestableType::Tree:
		// 木の破壊音
		Get<AudioManager>().Play(brokenTreeSE);

		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up(), Vec3::Up() * 0.05f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Left(), Vec3::Up() * 0.05f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Right(), Vec3::Up() * 0.05f);
		Get<EntitySystem>().Spawn(ITEM_ID_Mushroom, transform.position + Vec3::Up() + Vec3::Back(), Vec3::Up() * 0.05f);

		break;

	case HarvestableType::TreeHigh:
		// 背の高い木の破壊音
		Get<AudioManager>().Play(brokenTreeSE);

		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Left(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Right(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Back(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_WOOD, transform.position + Vec3::Up() + Vec3::Forward(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(
		    ITEM_ID_WOOD,
		    transform.position + Vec3::Up() + Vec3::Left() + Vec3::Back(),
		    Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(
		    ITEM_ID_Mushroom,
		    transform.position + Vec3::Up() + Vec3::Right() + Vec3::Forward(),
		    Vec3::Up() * 0.05f);
		Get<EntitySystem>().Spawn(
		    ITEM_ID_Mushroom,
		    transform.position + Vec3::Up() + Vec3::Right() + Vec3::Back(),
		    Vec3::Up() * 0.05f);
		break;

	case HarvestableType::Rock:
		// 岩の破壊音
		Get<AudioManager>().Play(brokenRockSE);

		Get<EntitySystem>().Spawn(ITEM_ID_ROCK, transform.position + Vec3::Up(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_ROCK, transform.position + Vec3::Up() + Vec3::Left(), Vec3::Up() * 0.0f);
		Get<EntitySystem>().Spawn(ITEM_ID_ROCK, transform.position + Vec3::Up() + Vec3::Right(), Vec3::Up() * 0.0f);
		break;

	default:
		break;
	}
}
