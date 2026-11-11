#pragma once
#include <Game/Game.h>

/// @brief チュートリアルのフェーズ番号
using TutorialPhaseType = int32_t;

/// @brief チュートリアルのフェーズ番号
enum TutorialPhaseType_ : TutorialPhaseType
{
	TutorialPhaseType_CheckBodyMove,
	TutorialPhaseType_CheckCameraMove,
	TutorialPhaseType_CheckOpenBackpack,

	TutorialPhaseType_Max,
};

using TutorialPhaseFlags = std::bitset<TutorialPhaseType_Max>;
