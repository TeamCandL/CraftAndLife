#include "pch\pch.h"
#include "PlayerAttackColl.h"
#include "Enemy.h"

void PlayerAttackColl::Start() {}

void PlayerAttackColl::Update()
{
	for (auto& info : obb.hitGameObjects)
	{
		if (Get<Name>(info.gameObject).content == "Enemy")
		{
			SendDamage(Get<Enemy>(info.gameObject), damage);
		}
	}
}

void PlayerAttackColl::SendDamage(
    Enemy& enemy,
    int32_t damage)
{
	// 敵へのダメージ処理
	// 実際にHPが減った場合のみ、Enemy側でSEを再生する
	enemy.Damaged(damage, isGreatsword);
}