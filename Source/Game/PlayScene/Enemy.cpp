#include "Enemy.h"
#include "EnemyAttackColl.h"
#include "pch\pch.h"
#include <shigaRandom/SgRandom.h>

void Enemy::Start()
{
	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>();
}

void Enemy::Update()
{
	/// 徘徊時のチェックポイントの更新
	if (objWanderingCheckPoint.IsEmpty())
	{
		// 1+1=11
		1 + 1;
	}
	else
	{
		// チェックポイントに到達したら新しいチェックポイントを生成する
		if ((transform.position - Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPosition()).Length() < 1.0f)
		{
			SetWanderingCheckPoint(Vec3{
			    WAN_REFERENCE - Random::GetRandom(WAN_RANGE - WAN_REFERENCE * 2), 0,
			    WAN_REFERENCE - Random::GetRandom(WAN_RANGE - WAN_REFERENCE * 2)});
		}
	}

	ImGui::Begin("Enemy");

	ImGui::DragInt("modelIndex", &modelIndex);
	modelIndex = std::clamp(modelIndex, 0I32, static_cast<int32_t>(models.size()) - 1);
	ImGui::DragInt("animIndex", &animIndex, 1, 0, 0);

	ImGui::End();
}

void Enemy::Draw()
{
	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[modelIndex].Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{models[modelIndex].Get().subParts};
	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void Enemy::AttackCollider()
{
	// for (auto& obj : Get<Player>())	//全てのプレイヤーを取得
	//{
	//	if ()	//一定範囲内にプレイヤーがいる場合、攻撃する
	//	{
	//		//new EnemyAttackColl();
	//	}
	// }
}

void Enemy::Damaged(int32_t damage)
{
	hp -= damage;
}

void Enemy::SetWanderingCheckPoint(const Vec3& _checkPoint)
{
	GetScene().Destroy(objWanderingCheckPoint);

	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>();
}
