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
	else
	{
		// VCU_LOG("{}がない！", enemyId);
	}
}

void OtherEnemy::Draw()
{
	transform.UpdateFamily();

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

Vec3 OtherEnemy::GetPosition() const
{
	return transform.position;
}

float OtherEnemy::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}
