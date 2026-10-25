
#include "pch/pch.h"
#include "Harvestable.h"

/// @brief 初期化
void Harvestable::Start()
{
	// 種類に応じたモデルと耐久値を設定
	switch (type)
	{
	case HarvestableType::Tree:
		model         = Resource<Model>{ "Model/Harvestable/Tree.fbx" };
		maxDurability = 3;
		break;

	case HarvestableType::TreeHigh:
		model         = Resource<Model>{ "Model/Harvestable/TreeHigh.fbx" };
		maxDurability = 5;
		break;

	case HarvestableType::Rock:
		model         = Resource<Model>{ "Model/Harvestable/Rock.fbx" };
		maxDurability = 5;
		break;

	default:
		// 未定義の種類は通常の木にする
		model         = Resource<Model>{ "Model/Harvestable/Tree.fbx" };
		maxDurability = 3;
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

	// 耐久値が0以下になったら採取済みにする
	if (durability <= 0)
	{
		durability  = 0;
		isHarvested = true;
	}
}

/// @brief 採取済みかどうかを取得する
bool Harvestable::IsHarvested() const
{
	return isHarvested;
}
