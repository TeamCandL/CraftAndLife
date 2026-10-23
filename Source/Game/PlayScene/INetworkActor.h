#pragma once
#include <Game/Game.h>

/// @brief ネットワークアクターのインタフェース
class INetworkActor
{
public:
	INetworkActor() = default;
	virtual ~INetworkActor() = default;

	/// @brief このネットワークアクターがホストであるか
	/// @return ホストである true / false
	virtual bool IsHost() const = 0;
};