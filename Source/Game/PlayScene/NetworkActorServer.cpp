#include "pch\pch.h"
#include "NetworkActorServer.h"
#include "../Manager/SessionManager.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "OtherEnemyController.h"

void NetworkActorServer::Start()
{
	// ゲストなら敵を生成しない
	if (!IsHost())
	{
		return;
	}

	CreateEnemies();
	// 敵管理オブジェクトを生成して保存
	Data().objEnemyController = GetScene().Instantiate<EnemyController>();

	// 敵を生成
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Skeleton));
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::BossSkeleton));
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Spider));
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Bat));
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Dragon));

	// 生成済みにする
	Data().isInitialized = true;
}

void NetworkActorServer::Update()
{
	// ホスト情報を更新
	Data().isHost = Get<SessionManager>().RefStatus().imLobbyHost;

	// ホストになったとき、まだ敵を生成していなければ生成
	if (IsHost())
	{
		CreateEnemies();

		Data().enemyStateSyncTimer -= Get<DeltaTime>();

		if (Data().enemyStateSyncTimer <= 0.0f)
		{
			Data().enemyStateSyncTimer += Data().enemyStateSyncTime;
			Data().isFullSync = true;
		}
	}
	else
	{
		// ゲストなら他の敵を同期するコントローラーを生成
		if (Data().objOtherEnemyController.IsEmpty())
		{
			Data().objOtherEnemyController = GetScene().Instantiate<OtherEnemyController>();
		}
	}
}

void NetworkActorServer::CreateEnemies()
{
	if (IsInitialized())
	{
		return;
	}

	// 敵管理オブジェクトを生成して保存
	Data().objEnemyController = GetScene().Instantiate<EnemyController>();

	// 敵を生成
	GameObject objEnemy = GetScene().Instantiate<Enemy>(Data().objEnemyController);

	// 敵のステート登録システムを生成
	EnemyStateRegistries(objEnemy);

	// 生成済みにする
	Data().isInitialized = true;
}

void NetworkActorServer::EnemyStateRegistries(
    GameObject objEnemy)
{
	auto& enemy           = Get<Enemy>(objEnemy);
	const int32_t enemyId = Get<Enemy>(objEnemy).GetID();

	if (enemyId >= 0)
	{
		auto& registries   = Data().enemyStateRegistries;
		const size_t index = static_cast<size_t>(enemyId);

		if (registries.size() <= static_cast<size_t>(enemyId))
		{
			registries.resize(static_cast<size_t>(enemyId) + 1);
		}

		if (!registries[enemyId].has_value())
		{
			registries[enemyId].emplace();
			enemy.RegisterStates(*registries[index]);
		}
	}
}

bool NetworkActorServer::IsHost()
{
	return Get<SessionManager>().RefStatus().imLobbyHost;
}
