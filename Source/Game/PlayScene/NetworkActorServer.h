#pragma once
#include <Game/Game.h>

/// @brief サーバー側のネットワークアクターのデータ
struct NetworkActorServerData
{
	bool isHost{ false };
	bool isInitialized{ false };

	GameObject objEnemyController{};
	GameObject objOtherEnemyController{};
};

/// @brief サーバー側のネットワークアクター
struct NetworkActorServer : ObjectBase<NetworkActorServerData>
{
	using ObjectBase<NetworkActorServerData>::ObjectBase;

	void Start() override;
	void Update() override;

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
