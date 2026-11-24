#include "pch\pch.h"
#include "OtherEnemyHpBarBg.h"
#include "OtherEnemyHpBar.h"

void OtherEnemyHpBarBg::Start() {}

void OtherEnemyHpBarBg::Update()
{
	if (!Get<OtherEnemyHpBar>(ownerOtherEnemyHp).IsActive())
	{
		SetActive(false);
		DestroyMe();
	}
}

void OtherEnemyHpBarBg::SetTargetPosition(
    const Vec3& _position)
{
	transform.position = _position + m_offset;
}
