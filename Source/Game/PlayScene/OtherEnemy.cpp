#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdateEnemyDamage.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>
#include "EnemyHpBar.h"
#include "Enemy.h"

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

		// 座標と角度の更新
		transform.position = status.position;
		transform.rotation = Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, status.rotation.y, 0.0f });

		bool alive = true;
		int32_t hp = 0;

		SessionStateReader reader{ sessionManager };

		if (reader.TryGetState(StateTarget::Enemy(enemyId), "hp", hp))
		{
			VCU_LOG("OtherEnemy ID:{} HP取得成功: {}", enemyId, hp);
		}

		if (reader.TryGetState(StateTarget::Enemy(enemyId), "alive", alive))
		{
			if (!alive)
			{
				VCU_LOG("OtherEnemy ID:{} is dead. Destroying...", enemyId);

				DestroyMe();
				return;
			}
		}

		for (const auto& [name, value] : status.states)
		{
			VCU_LOG("Enemy ID:{} State:{}", enemyId, name);
		}

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
	transform.UpdateFamily();

	SessionManager& sessionManager = Get<SessionManager>();

	if (sessionManager.IsExistEnemy(enemyId))
	{
		const auto& status = sessionManager.RefEnemyStatus(enemyId);

		VCU_LOG(
		    "OtherEnemy ID:{} Sync: ({}, {}, {}) Draw: ({}, {}, {})",
		    enemyId,
		    status.position.x,
		    status.position.y,
		    status.position.z,
		    transform.position.x,
		    transform.position.y,
		    transform.position.z);
	}

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