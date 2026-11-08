#include "pch\pch.h"
#include "NetworkActorServer.h"
#include "../Manager/SessionManager.h"
#include "Player.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "OtherEnemyController.h"
#include "DayController.h"

void NetworkActorServer::Start()
{
	// プレイヤーのステートを登録
	// if (!Data().isPlayerStateRegistered && !Data().objPlayer.IsEmpty())
	//{
	//	Player& player = Get<Player>(Data().objPlayer);
	//	player.RegisterStates(Data().playerStateRegistry);

	//	Data().isPlayerStateRegistered = true;
	//}

	// ゲストなら敵を生成しない
	if (!IsHost())
	{
		return;
	}

	CreateEnemies();
}

void NetworkActorServer::Update()
{
	// ホスト情報を更新
	Data().isHost = Get<SessionManager>().RefStatus().imLobbyHost;

	// ホスト・ゲスト共通：自分のプレイヤーステートを登録
	if (!Data().isPlayerStateRegistered && !Data().objPlayer.IsEmpty())
	{
		Player& player = Get<Player>(Data().objPlayer);
		player.RegisterStates(Data().playerStateRegistry);

		Get<SessionManager>().SetSelfStateRegistry(&Data().playerStateRegistry);

		VCU_LOG("プレイヤーステート登録完了: {}", Data().playerStateRegistry.GetAllStates().size());

		Data().isPlayerStateRegistered = true;
	}

	// ホスト専用処理
	if (IsHost())
	{
		// 日付の取得確認
		DayController& dayController = Get<DayController>(Data().objDayController);
		VCU_LOG("現在の日付: {}", dayController.GetCurrentDay());

#if 1
		if (dayController.IsNight())
		{
			CreateEnemies();
		}

#elif 0
		CreateEnemies();

#endif

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

void NetworkActorServer::CreateEnemy(
    EnemyType type)
{
	GameObject objEnemy = GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(type));

	// Data().objEnemyController = GetScene().Instantiate<EnemyController>();

	// Get<EnemyController>(Data().objEnemyController).SetNetworkActorServer(ToGameObject());

	// 生成した敵のステートを登録
	EnemyStateRegistries(objEnemy);
}

void NetworkActorServer::CreateEnemy(
    EnemyType type,
    const Vec3& position)
{
	GameObject objEnemy = GetScene().Instantiate<Enemy>(Data().objEnemyController, position, static_cast<int>(type));

	EnemyStateRegistries(objEnemy);
}

void NetworkActorServer::CreateEnemies()
{
	if (IsInitialized())
	{
		return;
	}

	// 敵管理オブジェクトを生成
	Data().objEnemyController = GetScene().Instantiate<EnemyController>();

	// 自分自身のGameObjectをEnemyControllerに渡す
	Get<EnemyController>(Data().objEnemyController).SetNetworkActorServer(ToGameObject());

	// 敵を生成（同じ種類も複数生成可能）
	// CreateEnemy(EnemyType::Skeleton);
	CreateEnemy(EnemyType::Skeleton, Vec3{ 1.0f, 7.0f, 2.0f });
	// CreateEnemy(EnemyType::Spider, Vec3{ -1.0f, 7.0f, 2.0f });
	CreateEnemy(EnemyType::Spider, Vec3{ 1.0f, 7.0f, -2.0f });
	// CreateEnemy(EnemyType::BossSkeleton, Vec3{ 1.0f, 5.0f, 2.0f });
	// CreateEnemy(EnemyType::Bat, Vec3{ 1.0f, 7.0f, 5.0f });
	// CreateEnemy(EnemyType::Dragon, Vec3{ 3.0f, 7.0f, 2.0f });

	//  生成済みにする
	Data().isInitialized = true;
}

void NetworkActorServer::EnemyStateRegistries(
    GameObject objEnemy)
{
	auto& enemy           = Get<Enemy>(objEnemy);
	const int32_t enemyId = enemy.GetID();

	if (enemyId < 0)
	{
		return;
	}

	auto& registries   = Data().enemyStateRegistries;
	const size_t index = static_cast<size_t>(enemyId);

	if (registries.size() <= index)
	{
		registries.resize(index + 1);
	}

	if (!registries[index].has_value())
	{
		registries[index].emplace();
		enemy.RegisterStates(*registries[index]);

		auto& queued = Data().enemyDeathQueued;

		if (queued.size() <= index)
		{
			queued.resize(index + 1, false);
		}

		queued[index] = false;
	}
}

void NetworkActorServer::MarkDeathQueued(
    int32_t actorId)
{
	if (actorId < 0)
	{
		return;
	}

	auto& queued       = Data().enemyDeathQueued;
	const size_t index = static_cast<size_t>(actorId);

	if (queued.size() <= index)
	{
		queued.resize(index + 1, false);
	}

	queued[index] = true;
}

bool NetworkActorServer::IsDeathQueued(
    int32_t actorId) const
{
	if (actorId < 0)
	{
		return false;
	}

	const auto& queued = Data().enemyDeathQueued;
	const size_t index = static_cast<size_t>(actorId);

	return index < queued.size() && queued[index];
}

void NetworkActorServer::OnEnemyDeath(
    int32_t actorId)
{
	// すでに死亡通知を登録済みなら何もしない
	if (actorId < 0 || IsDeathQueued(actorId))
	{
		return;
	}

	// 死亡通知を送信キューに登録
	// TODO: 死亡通知プロトコルの送信処理

	// 敵のステート登録を解除
	ReleaseActor(actorId);

	MarkDeathQueued(actorId);
}

void NetworkActorServer::ReleaseActor(
    int32_t actorId)
{
	if (actorId < 0)
	{
		return;
	}

	const size_t index = static_cast<size_t>(actorId);
	auto& registries   = Data().enemyStateRegistries;

	if (index < registries.size() && registries[index].has_value())
	{
		registries[index]->Clear();
		registries[index].reset();
	}

	// auto& queued = Data().enemyDeathQueued;

	// if (index < queued.size())
	//{
	//	queued[index] = false;
	// }
}
