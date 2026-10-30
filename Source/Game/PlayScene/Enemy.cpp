#include "Enemy.h"
#include "EnemyAttackColl.h"
#include "pch\pch.h"
#include <Game/System/AudioSystem.h>
#include <shigaLibrary/SgRandom.h>
#include <shigaLibrary/SgMath.h>
#include <Game/PlayScene/TerrainSystem.h>

#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include "EnemyController.h"
#include "../Manager/SessionManager.h"

void Enemy::Start()
{
	Get<EnemyController>(objEnemyController).AddEnemy(ToGameObject());

	SetWanderingCheckPoint(transform.position);
	nowAttackCoolTime  = ATTACK_COOL_TIME;
	nowDamagedCoolTime = DAMAGED_COOL_TIME;

	obb.isTrigger = true;  ///< 当たり判定を貫通させる(押し出さなくなる)
}

void Enemy::Update()
{
	ImGui::Begin("Enemy");
	ImGui::DragFloat3("position", transform.position.v);
	ImGui::DragFloat3("scale", transform.scale.v);
	ImGui::End();

	if (hp <= 0)
	{
		SetAlive(false);
	}

	// 徘徊チェックポイントとの距離(差分)
	if (!followed)
	{
		targetPos = Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPositionVec2();
	}

	Sensor();

	VCU_LOG("敵の追跡状況：{}", followed);

	float distX = targetPos.x - transform.position.x;
	float distZ = targetPos.y - transform.position.z;
	float vecX  = Math::GetSquareVec(distX, distZ);
	float vecZ  = Math::GetSquareVec(distZ, distX);
	// 差分を基に移動方向の計算
	angleY = Math::GetAngle(distX, distZ);

	Move(vecX, vecZ, speed, angleY);

	nowAttackCoolTime -= Get<DeltaTime>();
	if (nowAttackCoolTime <= 0.0f)
	{
		AttackCollider();
	}
	nowDamagedCoolTime -= Get<DeltaTime>();

	//	徘徊時のチェックポイントの更新
	if (!followed)
	{
		if (!objWanderingCheckPoint.IsEmpty())
		{
			// チェックポイントに到達したら新しいチェックポイントを生成する
			if ((transform.position - Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPositionVec3()).Length() < 1.0f)
			{
				SetWanderingCheckPoint(transform.position);
			}
		}
		else
		{
			SetWanderingCheckPoint(transform.position);
		}
	}

	ImGui::Begin(std::format("Enemy:{}", nameDEB.content).c_str());

	ImGui::DragInt("modelIndex", &modelIndex);
	modelIndex = std::clamp(modelIndex, 0I32, static_cast<int32_t>(models.size()) - 1);
	ImGui::DragInt("animIndex", &animIndex, 1, 0, 0);

	ImGui::End();

	if (!alive)
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
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	transform.rotation = Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, angleY_, 0.0f });
	transform.position.x += vecX_ * spd_ * Get<DeltaTime>();
	transform.position.z += vecZ_ * spd_ * Get<DeltaTime>();
}

void Enemy::Sensor()
{
	Vec2 nowVec2Pos{ transform.position.x, transform.position.z };
	Vec2 targetPos_{ DEFAULT_TARGET_POS };
	float targetDist{ -1.0f };
	std::vector<Vec2> playerPositions;
	Get<SessionManager>().ForeachPlayerStatus(
	    [&playerPositions](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    playerPositions.push_back({ _status.position.x, _status.position.z });
	    });
	for (Vec2 objPlayerPos : playerPositions)
	{
		float playerDist = (nowVec2Pos - objPlayerPos).Length();
		if (playerDist <= visibilityDist)
		{
			Vec2 dist;
			dist.x      = nowVec2Pos.x - objPlayerPos.x;
			dist.y      = nowVec2Pos.y - objPlayerPos.y;
			float angle = Math::GetDeg(angleY) - Math::GetDeg(atan2f(dist.x, dist.y));
			VCU_LOG("AngleE : {}, AngleT : {}, angle : {}", angleY, atan2f(dist.x, dist.y), angle);
			if (abs(angle) <= visibilityAngle / 2.0f)
			{
				if (targetDist < 0.0f)
				{
					targetPos_ = objPlayerPos;
					targetDist = playerDist;
				}
				else
				{
					if (targetDist > playerDist)
					{
						targetPos_ = objPlayerPos;
						targetDist = playerDist;
					}
				}
			}
		}
	}
	followed = false;
	if (targetPos_ != DEFAULT_TARGET_POS)
	{
		followed = true;

		if (!objWanderingCheckPoint.IsEmpty())
		{
			GetScene().Destroy(objWanderingCheckPoint);
		}
		targetPos = targetPos_;
	}
}

void Enemy::Draw()
{
	VCU_LOG("hostEnemy Position: ({}, {}, {})", transform.position.x, transform.position.y, transform.position.z);

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

void Enemy::OnDestroy()
{
	int a = 10;
	a++;
}

void Enemy::AttackCollider()
{
	if (nowAttackCoolTime <= 0.0f)
	{
		attacked = true;
		Get<AudioSystem>().Play(attackSE);

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

	Vec3 range_{ WAN_REFERENCE - Random::GetRandom(WAN_RANGE), 0.0f, WAN_REFERENCE - Random::GetRandom(WAN_RANGE) };

	position_ += range_;

	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>(position_);
}
