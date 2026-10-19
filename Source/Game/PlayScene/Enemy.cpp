#include "Enemy.h"
#include "EnemyAttackColl.h"
#include "pch\pch.h"
#include <shigaRandom/SgRandom.h>

void Enemy::Start()
{
	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>();
}

void Enemy::Update()
{
	/// 徘徊時のチェックポイントの更新
	if (objWanderingCheckPoint.IsEmpty())
	{
	}
	else
	{
		// チェックポイントに到達したら新しいチェックポイントを生成する
		if ((transform.position - Get<EnemyCheckPoint>(objWanderingCheckPoint).GetPosition()).Length() < 1.0f)
		{
			SetWanderingCheckPoint(Vec3{WAN_REFERENCE - Random::GetRandom(WAN_RANGE - WAN_REFERENCE * 2), 0,
			                            WAN_REFERENCE - Random::GetRandom(WAN_RANGE - WAN_REFERENCE * 2)});
		}
	}
}

void Enemy::Draw()
{
}

void Enemy::AttackCollider()
{
	// for (auto& obj : Get<Player>())	//全てのプレイヤーを取得
	//{
	//	if ()	//一定範囲内にプレイヤーがいる場合、攻撃する
	//	{
	//		//new EnemyAttackColl();
	//	}
	// }
}

void Enemy::Damaged(int32_t damage)
{
	hp -= damage;
}

void Enemy::SetWanderingCheckPoint(const Vec3 &_checkPoint)
{
	GetScene().Destroy(objWanderingCheckPoint);

	objWanderingCheckPoint = GetScene().Instantiate<EnemyCheckPoint>();
}
