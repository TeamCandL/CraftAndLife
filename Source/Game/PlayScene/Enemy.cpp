#include "pch\pch.h"
#include "Enemy.h"
#include "EnemyAttackColl.h"

void Enemy::Start()
{
}

void Enemy::Update()
{
}

void Enemy::Draw()
{
}

void Enemy::AttackCollider()
{
	//new EnemyAttackColl();
}

void Enemy::Damaged(int32_t damage)
{
	hp -= damage;
}
