#pragma once
#include <Game/Game.h>

using HelthState = int32_t;
enum HelthState_
{
	HelthState_Live,  ///< 生きている
	HelthState_Dead,  ///< 死んでいる(観戦中)
};
