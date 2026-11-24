#pragma once

/// @brief 方向の種類
using InputDirectionType = int32_t;
/// @brief 方向の種類
enum InputDirectionType_ : InputDirectionType
{
	InputDirectionType_Up,        ///< 上向き
	InputDirectionType_Down,      ///< 下向き
	InputDirectionType_Left,      ///< 左向き
	InputDirectionType_Right,     ///< 右向き
	InputDirectionType_LeftTab,   ///< 左向きタブ
	InputDirectionType_RightTab,  ///< 右向きタブ
	InputDirectionType_Max,       ///< 方向の種類最大数
};
