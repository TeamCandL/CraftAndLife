#pragma once
#include <Game/Game.h>

/// @brief サーバー側のネットワークアクターのデータ
struct NetworkActorServerData
{
	bool isHost{ false };
};

/// @brief サーバー側のネットワークアクター
struct NetworkActorServer : ObjectBase<NetworkActorServerData>
{
	void Start() override;
	void Update() override;

	bool IsHost() const
	{
		return Data().isHost;
	}
};
