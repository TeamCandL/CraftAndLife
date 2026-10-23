#include "pch\pch.h"
#include "EnemyAttackColl.h"
#include "Player.h"

void EnemyAttackColl::Start()
{
}

void EnemyAttackColl::Update()
{
	///	obb.hitGameObjects→このオブジェクトの持つコライダーに触れているオブジェクトの情報が入っている。
	///	※ただ、これ自体はオブジェクトを取得しているわけではないため、情報に対して『.gameObject』をつけることでオブジェクトを取得する。
	for (auto& info : obb.hitGameObjects)
	{
		///	取得したオブジェクトの名前を『.content』で取得。
		///	※ここでは、それがプレイヤーであるかを判定している。
		if (Get<Name>(info.gameObject).content == "Player")
		{
			SendDamage(Get<Player>(info.gameObject), damage);
		}
	}
}

void EnemyAttackColl::SendDamage(Player& player, int32_t damage)
{
	player.Damaged(damage);
}
