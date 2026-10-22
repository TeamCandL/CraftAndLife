#include "Enemy.h"
#include "EnemyAttackColl.h"
#include "pch\pch.h"
#include <shigaLibrary/SgRandom.h>
#include <shigaLibrary/SgMath.h>

void Enemy::Start()
{
	SetWanderingCheckPoint(transform.position);
	nowAttackCoolTime  = ATTACK_COOL_TIME;
	nowDamagedCoolTime = DAMAGED_COOL_TIME;
}

void Enemy::Update()
{
	ImGui::Begin("Enemy");
	ImGui::DragFloat3("position", transform.position.v);
	ImGui::DragFloat3("scale", transform.scale.v);
	ImGui::End();

	// 徘徊チェックポイントとの距離(差分)
	float distX = Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPosition().x - transform.position.x;
	float distZ = Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPosition().z - transform.position.z;
	float vecX  = Math::GetSquareVec(distX, distZ);
	float vecZ  = Math::GetSquareVec(distZ, distX);
	// 差分を基に移動方向の計算
	float angleY = Math::GetAngle(distX, distZ);

	// Move(vecX, vecZ, speed, angleY);

	nowAttackCoolTime -= Get<DeltaTime>();
	if (nowAttackCoolTime <= 0.0f)
	{
		AttackCollider();
	}
	nowDamagedCoolTime -= Get<DeltaTime>();

	//	徘徊時のチェックポイントの更新
	if (!objWanderingCheckPoint.IsEmpty())
	{
		// チェックポイントに到達したら新しいチェックポイントを生成する
		if ((transform.position - Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPosition()).Length() < 1.0f)
		{
			SetWanderingCheckPoint(transform.position);
		}
	}
	else
	{
	}

	ImGui::Begin("Enemy");

	ImGui::DragInt("modelIndex", &modelIndex);
	modelIndex = std::clamp(modelIndex, 0I32, static_cast<int32_t>(models.size()) - 1);
	ImGui::DragInt("animIndex", &animIndex, 1, 0, 0);

	ImGui::End();

	if (hp <= 0)
	{
		DestroyMe();
	}
}

void Enemy::Move(
    float vecX_,
    float vecZ_,
    float spd_,
    float angleY_)
{
	transform.rotation = Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, angleY_, 0.0f });
	transform.position.x += vecX_ * spd_ * Get<DeltaTime>();
	transform.position.z += vecZ_ * spd_ * Get<DeltaTime>();
}

void Enemy::Draw()
{
	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[modelIndex].Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{ models[modelIndex].Get().subParts };
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
	if (nowAttackCoolTime <= 0.0f)
	{
		GetScene().Instantiate<EnemyAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME);
		nowAttackCoolTime = ATTACK_COOL_TIME;
	}
}

void Enemy::Damaged(
    int32_t damage)
{
	if (nowDamagedCoolTime <= 0.0)
	{
		hp -= damage;
		nowDamagedCoolTime = DAMAGED_COOL_TIME;
	}
}

void Enemy::SetWanderingCheckPoint(
    Vec3 position_)
{
	if (objWanderingCheckPoint)
	{
		GetScene().Destroy(objWanderingCheckPoint);
	}

	Vec3 range_{ WAN_REFERENCE - Random::GetRandom(WAN_RANGE), 0, WAN_REFERENCE - Random::GetRandom(WAN_RANGE) };

	position_ += range_;

	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>(position_);
}
