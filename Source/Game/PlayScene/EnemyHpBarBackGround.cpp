#include "pch\pch.h"
#include "EnemyHpBarBackGround.h"
#include "EnemyHpBar.h"

void EnemyHpBarBg::Start() {}

void EnemyHpBarBg::Update()
{
	if (!Get<EnemyHpBar>(ownerEnemyHp).IsActive())
	{
		SetActive(false);
		DestroyMe();
	}
}

void EnemyHpBarBg::SetTargetPosition(
    const Vec3& _position)
{
	transform.position = _position + m_offset;
}
