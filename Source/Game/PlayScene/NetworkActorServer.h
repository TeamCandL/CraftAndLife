#pragma once
#include <Game/Game.h>
#include "EnemyStateRegistry.h"
#include <optional>
#include <vector>
#include "EnemyType.h"

/// @brief サーバー側のネットワークアクターのデータ
struct NetworkActorServerData
{
	bool isHost{ false };

	bool isInitialized{ false };

	GameObject objEnemyController{};
	GameObject objOtherEnemyController{};

	/// @brief 敵IDごとのステート登録情報
	std::vector<std::optional<EnemyStateRegistry>> enemyStateRegistries;
	/// @brief 敵ステートの全同期間隔（秒）
	static constexpr float enemyStateSyncTime{ 5.0f };
	/// @brief 次の全同期までの残り時間（秒）
	float enemyStateSyncTimer{ 0.0f };
	/// @brief 全同期フラグ
	bool isFullSync{ false };
};

/// @brief サーバー側のネットワークアクター
struct NetworkActorServer : ObjectBase<NetworkActorServerData>
{
	using ObjectBase<NetworkActorServerData>::ObjectBase;

	void Start() override;
	void Update() override;

	void CreateEnemy(EnemyType type);
	void CreateEnemy(EnemyType type, const Vec3& position);
	void CreateEnemies();
	void EnemyStateRegistries(GameObject objEnemy);

	bool IsHost() const
	{
		return Data().isHost;
	}

	// 敵管理オブジェクトを取得
	GameObject GetEnemyController() const
	{
		return Data().objEnemyController;
	}

	// 敵の生成が完了しているか確認
	bool IsInitialized() const
	{
		return Data().isInitialized;
	}

	// 敵ステート登録情報を取得
	std::vector<std::optional<EnemyStateRegistry>>& GetEnemyStateRegistries()
	{
		return Data().enemyStateRegistries;
	}

	const std::vector<std::optional<EnemyStateRegistry>>& GetEnemyStateRegistries() const
	{
		return Data().enemyStateRegistries;
	}

	// 全同期タイマーを取得
	float GetEnemyStateSyncTimer() const
	{
		return Data().enemyStateSyncTimer;
	}

	bool IsFullSync() const
	{
		return Data().isFullSync;
	}

	void ResetFullSync()
	{
		Data().isFullSync = false;
	}
};
