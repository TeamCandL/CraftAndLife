#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdateEnemyDamage.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>
#include "EnemyHpBar.h"
#include "Enemy.h"
#include "StateRegistry.h"

void OtherEnemy::Start()
{
	VCU_LOG("OtherEnemy Start: ID={}", enemyId);
	obb.isTrigger = true;
}

void OtherEnemy::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	if (sessionManager.IsExistEnemy(enemyId))
	{
		const InSessionStatus::EnemyStatus& status = sessionManager.RefEnemyStatus(enemyId);

		if (isDead)
		{
			UpdateDeathAnimation();
			return;
		}

		// 座標と角度の更新
		// transform.position = status.position;
		// transform.rotation = Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, status.rotation.y, 0.0f });

		// 最新のネットワーク座標
		targetPosition = status.position;

		const Quaternion targetRotation = Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, status.rotation.y, 0.0f });

		// 初回は即座に合わせる
		if (!isPositionInitialized)
		{
			transform.position = targetPosition;
			transform.rotation = targetRotation;

			previousPosition = targetPosition;
			previousRotation = targetRotation;

			isPositionInitialized = true;
		}
		else
		{
			// 位置を50%補間
			transform.position = (previousPosition + targetPosition) * 0.5f;

			// 回転を50%補間
			transform.rotation = Quaternion::Slerp(previousRotation, targetRotation, 0.5f);

			// 次回用に保存
			previousPosition = targetPosition;
			previousRotation = targetRotation;
		}

		bool alive = true;
		int32_t hp = 0;

		SessionStateReader reader{ sessionManager };

		/*if (reader.TryGetState(StateTarget::Enemy(enemyId), "hp", hp))
		{
		    VCU_LOG("OtherEnemy ID:{} HP取得成功: {}", enemyId, hp);
		}*/

		if (reader.TryGetState(StateTarget::Enemy(enemyId), "alive", alive))
		{
			if (!alive)
			{
				VCU_LOG("OtherEnemy ID:{} is dead. Destroying...", enemyId);

				// DestroyMe();
				StartDeathAnimation();
				return;
			}
		}

		/*for (const auto& [name, value] : status.states)
		{
		    VCU_LOG("Enemy ID:{} State:{}", enemyId, name);
		}*/

		// 被弾確認
		if (damagedFlag)
		{
			SendEnemyDamage(enemyId, damage);

			damagedFlag = false;
			damage      = 0;
		}

		if (Get<InputSystem>().GetKeyDown(KeyCode::F))
		{
			damage      = 10;
			damagedFlag = true;
			VCU_LOG("敵ダメージ送信: ID={}, Damage={}", enemyId, damage);
		}
	}
}

void OtherEnemy::Draw()
{
	// return;
	transform.UpdateFamily();

	SessionManager& sessionManager = Get<SessionManager>();

	// if (sessionManager.IsExistEnemy(enemyId))
	//{
	//	const auto& status = sessionManager.RefEnemyStatus(enemyId);

	//	VCU_LOG(
	//	    "OtherEnemy ID:{} Sync: ({}, {}, {}) Draw: ({}, {}, {})",
	//	    enemyId,
	//	    status.position.x,
	//	    status.position.y,
	//	    status.position.z,
	//	    transform.position.x,
	//	    transform.position.y,
	//	    transform.position.z);
	//}

	Get<Dx11Manager>().SetShader(shader);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[enemyType].Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{ models[enemyType].Get().subParts };
	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void OtherEnemy::SendEnemyDamage(
    int32_t enemyId,
    int32_t damage)
{
	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	Protocol::UpdateEnemyDamage message{};
	message.enemyId = enemyId;
	message.damage  = damage;

	networkSessionManager.Send(message);
}

void OtherEnemy::StartDeathAnimation()
{
	isDead          = true;
	deathElapsedSec = 0.0f;

	deathStartScale    = transform.scale;
	deathStartRotation = transform.rotation;
}

void OtherEnemy::UpdateDeathAnimation()
{
	deathElapsedSec += Get<DeltaTime>();

	// 前半：横たわる（0.6秒）
	float fallProgress = std::clamp(deathElapsedSec / fallDurationSec, 0.0f, 1.0f);

	if (fallProgress < 1.0f)
	{
		Quaternion fallRotation = Quaternion::FromEuler(Vec3{ 0.0f, 0.0f, DirectX::XM_PIDIV2 });

		transform.rotation = Quaternion::Slerp(deathStartRotation, deathStartRotation * fallRotation, fallProgress);
	}
	else
	{
		// 後半：縮小する（0.8秒）
		float shrinkProgress = std::clamp((deathElapsedSec - fallDurationSec) / shrinkDurationSec, 0.0f, 1.0f);

		float scaleRate = 1.0f - shrinkProgress;
		transform.scale = deathStartScale * scaleRate;

		if (shrinkProgress >= 1.0f)
		{
			DestroyMe();
		}
	}
}

void OtherEnemy::RegisterStates(
    StateRegistry& registry) const
{
	registry.Register("alive", !isDead);
}
