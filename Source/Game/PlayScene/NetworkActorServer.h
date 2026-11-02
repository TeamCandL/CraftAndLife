#pragma once
#include <Game/Game.h>
#include "StateRegistry.h"
#include <optional>
#include <vector>
#include "EnemyType.h"

/// @brief サーバー側のネットワークアクターのデータ
struct NetworkActorServerData
{
	bool isHost{ false };

	bool isInitialized{ false };

	GameObject objPlayer{};
	GameObject objEnemyController{};
	GameObject objOtherEnemyController{};

	/// @brief 自プレイヤーのステート登録情報
	StateRegistry playerStateRegistry;

	bool isPlayerStateRegistered{ false };

	/// @brief 敵IDごとのステート登録情報
	std::vector<std::optional<StateRegistry>> enemyStateRegistries;
	/// @brief 敵IDごとの死亡通知送信登録状態
	std::vector<bool> enemyDeathQueued;
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

	GameObject GetPlayer() const
	{
		return Data().objPlayer;
	}

	/// @brief プレイヤーのステート登録情報を取得
	StateRegistry& GetPlayerStateRegistry()
	{
		return Data().playerStateRegistry;
	}

	bool IsPlayerStateRegistered() const
	{
		return Data().isPlayerStateRegistered;
	}

	void SetPlayerStateRegistered(
	    bool value)
	{
		Data().isPlayerStateRegistered = value;
	}

	/// @brief 死亡通知の送信登録完了を記録
	void MarkDeathQueued(int32_t actorId);

	/// @brief 死亡通知が送信キューに登録済みか確認
	bool IsDeathQueued(int32_t actorId) const;

	/// @brief 指定した敵のステート登録情報を解除
	void ReleaseActor(int32_t actorId);

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
	std::vector<std::optional<StateRegistry>>& GetEnemyStateRegistries()
	{
		return Data().enemyStateRegistries;
	}

	const std::vector<std::optional<StateRegistry>>& GetEnemyStateRegistries() const
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
