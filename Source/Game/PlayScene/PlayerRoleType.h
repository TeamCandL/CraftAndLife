#pragma once
#include <Game/Game.h>

/// @brief プレイヤーの役職
using PlayerRoleType = int32_t;
/// @brief プレイヤーの役職
enum PlayerRoleType_ : PlayerRoleType
{
	PlayerRoleType_Healer,   ///< ヒーラー
	PlayerRoleType_Tank,     ///< タンク
	PlayerRoleType_Archer,   ///< ၐုပကက့နင
	PlayerRoleType_Fighter,  ///< 剣士
	PlayerRoleType_Max,
};
