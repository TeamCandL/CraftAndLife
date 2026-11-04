#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの役職タイプ
using PlayerJobType = int32_t;

/// @brief プレイヤーの役職タイプ
enum PlayerJobType_ : PlayerJobType
{
	PlayerJobType_Healer,   ///< 回復使い
	PlayerJobType_Tank,     ///< 盾使い
	PlayerJobType_Fighter,  ///< 大剣使い
	PlayerJobType_Archer,   ///< 弓使い
	PlayerJobType_Max,      ///< 最大数
};
