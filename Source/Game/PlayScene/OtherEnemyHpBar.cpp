#include "pch\pch.h"
#include "OtherEnemyHpBar.h"
#include "OtherEnemy.h"
#include <Game/System/CameraSystem.h>
#include <Base/Manager/ScreenSize.h>
#include <algorithm>
#include "OtherEnemyHpBarBg.h"
#include "../Manager/SessionManager/SessionStateReader.h"

void OtherEnemyHpBar::Start()
{
	OtherEnemy& enemy              = Get<OtherEnemy>(ownerEnemy);
	SessionManager& sessionManager = Get<SessionManager>();
	SessionStateReader reader(sessionManager);

	const int32_t enemyId = enemy.Data().enemyId;
	StateTarget target    = StateTarget::Enemy(enemy.Data().enemyId);
	int32_t _hp           = 0;
	reader.TryGetState(target, "hp", _hp);
	hp = maxHp = _hp;

	objBackGround = GetScene().Instantiate<OtherEnemyHpBarBg>(this->ToGameObject(), m_worldPosition);
}

void OtherEnemyHpBar::Update()
{
	// 敵から最新のHPと座標を同期
	if (!ownerEnemy.IsEmpty())
	{
		OtherEnemy& enemy              = Get<OtherEnemy>(ownerEnemy);
		SessionManager& sessionManager = Get<SessionManager>();
		SessionStateReader reader(sessionManager);
		const InSessionStatus::EnemyStatus& status = sessionManager.RefStatus().enemiesStatus.at(enemy.Data().enemyId);
		StateTarget target                         = StateTarget::Enemy(enemy.Data().enemyId);
		int32_t _hp                                = 0;
		reader.TryGetState(target, "hp", _hp);
		SetHp(_hp);
		SetTargetPosition(status.position);
	}

	if (hp <= 0)
	{
		SetActive(false);
		DestroyMe();
	}
}

void OtherEnemyHpBar::SetTargetPosition(
    const Vec3& position)
{
	transform.position = position;
	transform.position.y += m_offset.y;
	Get<OtherEnemyHpBarBg>(objBackGround).SetTargetPosition(position);
}

void OtherEnemyHpBar::SetHp(
    const int32_t _hp)
{
	hp = _hp;
	if (maxHp > 0)
	{
		hpRatio = static_cast<float>(hp) / static_cast<float>(maxHp);
	}
	else
	{
		hpRatio = 1.0f;
	}
	hpRatio              = std::clamp(hpRatio, 0.0f, 1.0f);
	barTransform.scale.x = BAR_WIDTH * hpRatio;
}