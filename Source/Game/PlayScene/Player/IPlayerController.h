#pragma once
#include <Game/Game.h>

struct PlayerData;

/// @brief プレイヤーの操作
class IPlayerController
{
public:
	IPlayerController() = default;
	~IPlayerController() = default;

	/// @brief 更新処理
	virtual void Update(PlayerData&) = 0;
};
