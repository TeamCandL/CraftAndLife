#pragma once
#include <Game/Game.h>

/// @brief チュートリアルのフェーズ番号
using TutorialElementType = int32_t;

/// @brief チュートリアルのフェーズ番号
enum TutorialElementType_ : TutorialElementType
{
	TutorialElementType_CheckBodyMove,      ///< プレイヤーの移動があったか
	TutorialElementType_CheckCameraMove,    ///< プレイヤーのカメラの移動があったか
	TutorialElementType_CheckOpenBackpack,  ///< バックパックを開いたことがあるか

	TutorialElementType_Max,
};

using TutorialElementFlags = std::bitset<TutorialElementType_Max>;

/// @brief チュートリアルでフォーカスする必要がある種類
using TutorialFocusType = int32_t;
enum TutorialForcusType_ : TutorialFocusType
{
	TutorialForcusType_MoveCamera,  ///< カメラ移動
	TutorialForcusType_MoveBody,    ///< ボディ移動
	TutorialForcusType_None_Max,    ///< 特に何もない 集中タイプの最大数
};
