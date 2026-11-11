#include "Enemy.h"
#include "EnemyAttackColl.h"
#include "pch\pch.h"
#include <Game/Manager/AudioManager.h>
#include <shigaLibrary/SgRandom.h>
#include <shigaLibrary/SgMath.h>
#include <Game/PlayScene/TerrainSystem.h>
#include <Game/PlayScene/NetworkActorServer.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>

#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include "EnemyController.h"
#include "StateRegistry.h"
#include "../Manager/SessionManager.h"
#include "EntitySystem.h"
#include "../PlayScene/Player/HelthState.h"

void Enemy::Start()
{
	Get<EnemyController>(objEnemyController).AddEnemy(ToGameObject());

	SetWanderingCheckPoint(transform.position);
	nowAttackCoolTime  = ATTACK_COOL_TIME;
	nowDamagedCoolTime = DAMAGED_COOL_TIME;

	obb.offset    = { 0.0f, 1.0f, 0.5f };
	obb.isTrigger = true;  ///< 当たり判定を貫通させる(押し出さなくなる)
}

void Enemy::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::O))
	{
		hp -= 10;
	}

	if (!alive)
		return;

	attacked = false;

	ImGui::Begin("Enemy");
	ImGui::DragFloat3("position", transform.position.v);
	ImGui::DragFloat3("scale", transform.scale.v);
	ImGui::DragInt("HP", &hp);
	ImGui::End();

	if (hp <= 0)
	{
		SetAlive(false);
		DropItem();
	}

	// VCU_LOG("敵の追跡状況：{}", followed);
	Move(targetPos, transform.position, transform.Down(), speed);

	Sensor(transform.Down());

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
			if ((targetPos - transform.position).Length() < 1.0f)
			{
				GetScene().Destroy(objWanderingCheckPoint);
				SetWanderingCheckPoint(transform.position);
			}
		}
		else
		{
			SetWanderingCheckPoint(transform.position);
		}
		targetPos = Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPositionVec3();
	}

	ImGui::Begin(std::format("Enemy:{}", nameDEB.content).c_str());

	ImGui::DragInt("modelIndex", &modelIndex);
	modelIndex = std::clamp(modelIndex, 0I32, static_cast<int32_t>(models.size()) - 1);
	ImGui::DragInt("animIndex", &animIndex, 1, 0, 0);

	ImGui::End();

	SessionManager& sessionManager = Get<SessionManager>();
	SessionStateReader reader{ sessionManager };

	reader.ForeachPlayer(
	    [&](const std::string& playerName, const StateTarget& target)
	    {
		    int32_t healthState{};

		    VCU_LOG("プレイヤー確認開始: {}", playerName);

		    if (!reader.TryGetState(target, "helthState", healthState))
		    {
			    VCU_LOG("プレイヤーの生存状態取得失敗: {}", playerName);
			    return;
		    }

		    VCU_LOG("プレイヤーの生存状態取得成功: 名前={}, helthState={}", playerName, healthState);

		    if (healthState == HelthState_Dead)
		    {
			    VCU_LOG("死亡プレイヤーのため追跡対象外: {}", playerName);
			    return;
		    }

		    VCU_LOG("生存プレイヤー確認: {}", playerName);

		    // int32_t healthState{};

		    // if (!reader.TryGetState(target, "helthState", healthState))
		    //  return;

		    // if (healthState == HelthState_Dead)
		    //  return;

		    // 生存プレイヤーとして追跡対象に追加
	    });

	if (alive)
	{
	}
	else
	{
		const int32_t enemyId = GetID();
		VCU_LOG("[Enemy] 死亡開始 ID={}", enemyId);

		SetActive(false);

		EnemyController& controller = Get<EnemyController>(objEnemyController);

		NetworkActorServer& networkActor = Get<NetworkActorServer>(controller.GetNetworkActorServer());

		// ステート登録解除・死亡通知登録
		networkActor.OnEnemyDeath(enemyId);

		// 敵の管理スロットを空にする
		controller.RemoveEnemy(enemyId);

		DestroyMe();

		VCU_LOG("[Enemy] DestroyMe 呼び出し完了 ID={}", enemyId);
		return;
	}
}

/// @brief ターゲットの方向に移動する
/// @param target_ ターゲットの位置
/// @param pos_ 自身の位置
/// @param Fwd 自身の向いている方向
/// @param spd_ 自身の移動速度
void Enemy::Move(
    Vec3 target_,
    Vec3 pos_,
    Vec3 Fwd_,
    float spd_)
{
	// VCU_LOG("TargetPosition : {}, {}", target_.x, target_.z);
	auto& [tx, _, tz] = target_.v;
	Vec3 tPos{ tx, 0.0f, tz };
	auto& [ex, __, ez] = pos_.v;
	Vec3 ePos{ ex, 0.0f, ez };
	float length = (tPos - ePos).Length();

	if ((length > 1.0f && followed) || !followed)
	{
		Vec3 crossed = Math::GetCrossProduct(Fwd_, tPos - ePos);

		torque += crossed.y * Math::GetRad(rotationAnglePerSec);

		Vec3 angles        = transform.rotation.ToEuler();
		angles.x           = -DirectX::XM_PIDIV2;
		angles.y           = torque;
		angles.z           = 0.0f;
		transform.rotation = Quaternion::FromEuler(angles);

		Vec3 move{ (Fwd_)*speed };
		transform.position += move * Get<DeltaTime>();

		/*ImGui::Begin("moveVec");
		ImGui::DragFloat3("move", move.v);
		ImGui::End();*/
	}
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);
}

void Enemy::Sensor(
    Vec3 Fwd_)
{
	SessionManager& sessionManager = Get<SessionManager>();
	SessionStateReader reader(sessionManager);

	Vec3 targetPos_{ DEFAULT_TARGET_POS };
	float targetDist{ -1.0f };
	int32_t targetAttentionLevel{ 0 };
	std::vector<TargetInfo> players;
	sessionManager.ForeachPlayerStatus(
	    [&players](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    players.push_back({ _name, _status.position, _status.attentionLevel });
	    });
	for (TargetInfo objPlayer : players)
	{
		float playerDist = (objPlayer._pos - transform.position).Length();
		if (playerDist <= visibilityDist)
		{
			if (objPlayer._attentionLevel >= targetAttentionLevel)
			{
				if (objPlayer._attentionLevel > targetAttentionLevel)
				{
					targetPos_           = objPlayer._pos;
					targetDist           = playerDist;
					targetAttentionLevel = objPlayer._attentionLevel;
				}
				else if (targetDist < 0.0f)
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
			float dot           = Math::GetDotProduct(Fwd_, (objPlayer._pos - transform.position));
			float visibilityDot = std::cosf(visibilityAngle);
			if (dot <= visibilityDot)
			{
			}
		}

		int32_t playerId{};
		if (sessionManager.TryGetPlayerId(objPlayer._name, playerId))
		{
			StateTarget target = StateTarget::Player(playerId);

			int32_t helthState{};
			bool flag = reader.TryGetState(target, "helthState", helthState);
			if (flag)
			{
				if (helthState == HelthState_::HelthState_Live)
				{
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
		followed = false;
	}
}

void Enemy::Draw()
{
	if (!alive)
		return;

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

	// あとから修正
	// Get<AudioManager>().Play(attackSE);

	// 近距離攻撃用アイテム
	const ItemId ITEM_ID_CLOSE_ATTACK{ 21 };

	// 敵の位置に攻撃判定用エンティティを生成
	Get<EntitySystem>().Spawn(ITEM_ID_CLOSE_ATTACK, transform.position, Vec3{ 0.0f, 0.0f, 0.0f });
	nowAttackCoolTime = ATTACK_COOL_TIME;
}

void Enemy::AttackCollider(
    const Vec3& forward_)
{
	attacked = true;
	// あとから修正
	// Get<AudioManager>().Play(attackSE);
	const ItemId ITEM_ID_SPIDER_STRING{ 20 };

	Vec3 dir = forward_.Normalize() * 10.0f;
	if (dir.Length() <= FLT_EPSILON)
	{
		dir = Vec3::Forward();
	}

	// 蜘蛛の糸攻撃！
	Get<EntitySystem>().Spawn(ITEM_ID_SPIDER_STRING, transform.position, dir);

	// NOTE: エンティティへ移行しますた
	// GetScene().Instantiate<EnemyAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME * 40.0f,
	// forward_, 10.0f);
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
	if (!objWanderingCheckPoint.IsEmpty())
		GetScene().Destroy(objWanderingCheckPoint);

	Vec3 range_{ WAN_REFERENCE - Random::GetRandom(WAN_RANGE), 0.0f, WAN_REFERENCE - Random::GetRandom(WAN_RANGE) };

	Vec3 position = position_;
	position      = position + range_;

	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>(position);
	followed               = false;
}

void Enemy::DropItem()
{
	Get<EntitySystem>().Spawn(dropItemID, transform.position + Vec3::Up(), Vec3::Zero());
}

void Enemy::RegisterStates(
    StateRegistry& registry) const
{
	registry.Register("hp", hp);
	// registry.Register("followed", followed);
	registry.Register("alive", alive);
	registry.Register("attacked", attacked);
}
