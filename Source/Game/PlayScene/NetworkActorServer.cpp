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

	// 敵管理オブジェクトを生成して保存
	Data().objEnemyController = GetScene().Instantiate<EnemyController>();

	// 敵を生成
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Skeleton));
	GetScene().Instantiate<Enemy>(Data().objEnemyController, static_cast<int>(EnemyType::Spider));
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
		if (!IsInitialized())
		{
			// 敵管理オブジェクトを生成して保存
			Data().objEnemyController = GetScene().Instantiate<EnemyController>();

			// 敵を生成
			GetScene().Instantiate<Enemy>(Data().objEnemyController);

			// 生成済みにする
			Data().isInitialized = true;
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
