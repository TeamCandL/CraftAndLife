#include "OtherEnemy.h"
#include <Game/Manager/SessionManager.h>

void OtherEnemy::Start()
{
	VCU_LOG("OtherEnemy Start: ID={}", enemyId);
}

void OtherEnemy::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();
	if (sessionManager.IsExistEnemy(enemyId))
	{
		const InSessionStatus::EnemyStatus& status = sessionManager.RefEnemyStatus(enemyId);

		// 座標と角度の更新
		transform.position = status.position;
		transform.rotation = Quaternion::FromEuler(Vec3{ 0.0f, status.rotation.y, 0.0f });
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