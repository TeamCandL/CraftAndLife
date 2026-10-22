#include "pch\pch.h"
#include "EnemyAttackColl.h"
#include "Player.h"

void EnemyAttackColl::Start()
{
	obb.offset = OBB_OFFSET;
	obb.scale  = Vec3::One() * OBB_SCALE;
}

void EnemyAttackColl::Update()
{
	lifeTime -= Get<DeltaTime>();
	if (lifeTime <= 0.0)
	{
		DestroyMe();
	}
	///	obb.hitGameObjects→このオブジェクトの持つコライダーに触れているオブジェクトの情報が入っている。
	///	※ただ、これ自体はオブジェクトを取得しているわけではないため、情報に対して『.gameObject』をつけることでオブジェクトを取得する。
	for (auto& info : obb.hitGameObjects)
	{
		///	取得したオブジェクトの名前を『.content』で取得。
		///	※ここでは、それがプレイヤーであるかを判定している。
		Name* pName = TryGet<Name>(info.gameObject);
		if (pName && pName->content == "Player")
		{
			SendDamage(Get<Player>(info.gameObject), damage);
			// 触れているプレイヤーに対してダメージを与える
			/*
			SendDamage();
			*/
		}
	}
}

void EnemyAttackColl::SendDamage(
    Player& player,
    int32_t damage)
{
	player.Damaged(damage);
}
