#include "pch\pch.h"
#include "EnemyHpBarBackGround.h"
#include "EnemyHpBar.h"

void EnemyHpBarBg::Start() {}

void EnemyHpBarBg::Update()
{
	OutputDebugStringA("EnemyHpBarBg Update\n");

	auto& hpBar = Get<EnemyHpBar>(ownerEnemyHp);

	if (!hpBar.IsActive())
	{
		OutputDebugStringA("HPBar Inactive\n");
		// SetActive(false);
		DestroyMe();
		return;
	}
}

void EnemyHpBarBg::SetTargetPosition(
    const Vec3& _position)
{
	transform.position = _position + m_offset;
}
