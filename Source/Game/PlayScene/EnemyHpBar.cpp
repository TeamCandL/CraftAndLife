#include "pch\pch.h"
#include "EnemyHpBar.h"
#include "Enemy.h"
#include <Game/System/CameraSystem.h>
#include <Base/Manager/ScreenSize.h>
#include <algorithm>
#include "EnemyHpBarBackGround.h"

void EnemyHpBar::Start()
{
	Enemy& enemy = Get<Enemy>(ownerEnemy);
	hp = maxHp    = enemy.GetHp();
	objBackGround = GetScene().Instantiate<EnemyHpBarBg>(this->ToGameObject(), m_worldPosition);
	// 初期化処理
}

void EnemyHpBar::Update()
{
	// 敵から最新のHPと座標を同期
	if (!ownerEnemy.IsEmpty())
	{
		Enemy& enemy = Get<Enemy>(ownerEnemy);
		SetTargetPosition(enemy.GetPosition());
		SetHp(enemy.Data().hp);

		// if (maxHp > 0)
		//{
		//	hpRatio = static_cast<float>(hp) / static_cast<float>(maxHp);
		//	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f);
		// }
	}

	if (hp <= 0)
	{
		SetActive(false);
		DestroyMe();
	}
}

void EnemyHpBar::OnDestroy()
{
	if (!objBackGround.IsEmpty())
	{
		Get<EnemyHpBarBg>(objBackGround).SetActive(false);
		Get<EnemyHpBarBg>(objBackGround).DestroyMe();
	}
}

void EnemyHpBar::SetTargetPosition(
    const Vec3& position)
{
	transform.position = position;
	transform.position.y += m_offset.y;
	Get<EnemyHpBarBg>(objBackGround).SetTargetPosition(position);
}

void EnemyHpBar::SetHp(
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