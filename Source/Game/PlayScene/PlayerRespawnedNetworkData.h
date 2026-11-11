#pragma once
#include <Game/Game.h>

/// @brief プレイヤーが復活したときに送信するデータ
struct PlayerRespawnedData
{
	std::string playerName{};  ///< 復活させたプレイヤー名
	int32_t hp{};              ///< 復活したときの体力値
};
