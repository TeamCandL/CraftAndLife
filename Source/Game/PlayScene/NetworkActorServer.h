#pragma once
#include <Game/Game.h>
#include "EnemyStateRegistry.h"
#include <optional>
#include <vector>

/// @brief サーバー側のネットワークアクターのデータ
struct NetworkActorServerData
{
	bool isHost{ false };
	bool isInitialized{ false };

	GameObject objEnemyController{};
	GameObject objOtherEnemyController{};

	/// @brief 敵IDごとのステート登録情報
	std::vector<std::optional<EnemyStateRegistry>> enemyStateRegistries;
};

/// @brief サーバー側のネットワークアクター
struct NetworkActorServer : ObjectBase<NetworkActorServerData>
{
	using ObjectBase<NetworkActorServerData>::ObjectBase;

	void Start() override;
	void Update() override;

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
};
