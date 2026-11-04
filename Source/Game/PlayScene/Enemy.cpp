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
#include "StateRegistry.h"
#include "../Manager/SessionManager.h"
#include "EntitySystem.h"

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
	attacked = false;

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("position", transform.position.v);
	ImGui::DragFloat3("scale", transform.scale.v);
	ImGui::DragInt("HP", &hp);
	ImGui::End();

	if (hp <= 0)
	{
		SetAlive(false);
	}

	// 徘徊チェックポイントとの距離(差分)
	if (!followed)
	{
		targetPos = Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPositionVec3();
	}

	Sensor();

	targetPos.y = Get<TerrainSystem>().GetHeight(targetPos.x, targetPos.z);

	// VCU_LOG("敵の追跡状況：{}", followed);

	Move(targetPos, transform.position, transform.Forward(), speed);

	nowAttackCoolTime -= Get<DeltaTime>();
	if (nowAttackCoolTime <= 0.0f)
	{
		Attack(targetPos);
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
		// 敵IDを使って死亡通知を送信キューに登録
		// Registryの登録を解除
		// ※上記2つは今後実装する
		// DestroyMe();
	}
}

void Enemy::Move(
    Vec3 target_,
    Vec3 pos_,
    Vec3 Fwd,
    float spd_)
{
	// VCU_LOG("TargetPosition : {}, {}", target_.x, target_.z);
	auto& [tx, _, tz] = target_.v;
	Vec3 tPos{ tx, 0.0f, tz };
	auto& [ex, __, ez] = pos_.v;
	Vec3 ePos{ ex, 0.0f, ez };
	Vec3 crossed = Math::GetCrossProduct(Fwd, tPos - ePos);

	torque += crossed.y * Math::GetRad(rotationAnglePerSec);

	Vec3 angles        = transform.rotation.ToEuler();
	angles.y           = torque;
	transform.rotation = Quaternion::FromEuler(angles);

	Vec3 move{ transform.Forward() * speed };
	transform.position += move * Get<DeltaTime>();

	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);
}

void Enemy::Sensor()
{
	Vec3 targetPos_{ DEFAULT_TARGET_POS };
	float targetDist{ -1.0f };
	int32_t targetAttentionLevel{ 0 };
	std::vector<TargetInfo> players;
	Get<SessionManager>().ForeachPlayerStatus(
	    [&players](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    players.push_back({ _status.position, _status.attentionLevel });
	    });
	for (TargetInfo objPlayer : players)
	{
		float playerDist = (transform.position - objPlayer._pos).Length();
		if (playerDist <= visibilityDist)
		{
			float dot           = Math::GetDotProduct(transform.Forward(), (objPlayer._pos - transform.position));
			float visibilityDot = std::cosf(visibilityAngle);
			if (dot >= visibilityDot)
			{
				if (objPlayer._attentionLevel > targetAttentionLevel)
				{
					if (targetDist < 0.0f)
					{
						targetPos_           = objPlayer._pos;
						targetDist           = playerDist;
						targetAttentionLevel = objPlayer._attentionLevel;
					}
					else
					{
						if (targetDist > playerDist)
						{
							targetPos_           = objPlayer._pos;
							targetDist           = playerDist;
							targetAttentionLevel = objPlayer._attentionLevel;
						}
					}
				}
			}
			else
			{
			}
		}
	}
	if (targetPos_ != DEFAULT_TARGET_POS)
	{
		if (!objWanderingCheckPoint.IsEmpty())
		{
			GetScene().Destroy(objWanderingCheckPoint);
		}
		targetPos = targetPos_;
		followed  = true;
	}
	else
	{
		// followed = false;
	}
}

void Enemy::Draw()
{
	// VCU_LOG("hostEnemy Position: ({}, {}, {})", transform.position.x, transform.position.y, transform.position.z);

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

void Enemy::Attack(
    Vec3 targetPos_)
{
	if (followed)
	{
		if (nowAttackCoolTime <= 0.0f)
		{
			float dist = (targetPos_ - transform.position).Length();
			if (hasLongAttack && dist > 5.0f)
			{
				LongAttack();
			}
			else if (hasCloseAttack && dist <= 5.0f)
			{
				CloseAttack();
			}
		}
	}
}

void Enemy::LongAttack()
{
	Vec3 forward = targetPos - transform.position;
	AttackCollider(forward);
}

void Enemy::CloseAttack()
{
	AttackCollider();
}

void Enemy::AttackCollider()
{
	attacked = true;
	Get<AudioSystem>().Play(attackSE);

	GetScene().Instantiate<EnemyAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME);
	nowAttackCoolTime = ATTACK_COOL_TIME;
}

void Enemy::AttackCollider(
    const Vec3& forward_)
{
	attacked = true;
	Get<AudioSystem>().Play(attackSE);

	GetScene().Instantiate<EnemyAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME * 40.0f, forward_, 10.0f);
	nowAttackCoolTime = ATTACK_COOL_TIME;
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
	followed               = false;
}

void Enemy::DropItem()
{
	Get<EntitySystem>().Spawn(dropItemID, transform.position, { 0.0f, 0.0f, 0.0f });
}

void Enemy::RegisterStates(
    StateRegistry& registry) const
{
	registry.Register("hp", hp);
	// registry.Register("followed", followed);
	registry.Register("alive", alive);
	// registry.Register("attacked", attacked);
}
