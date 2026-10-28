#pragma once
#include <Game/Game.h>

/// @brief プレイヤーのアニメーションの種類
using PlayerAnimType = int32_t;

/// @brief プレイヤーのアニメーションの種類
enum PlayerAnimType_ : PlayerAnimType
{
	PlayerAnimType_Stop,         ///< 直立不動
	PlayerAnimType_Idle,         ///< 待機状態
	PlayerAnimType_Walk,         ///< 歩く
	PlayerAnimType_Run,          ///< 走る
	PlayerAnimType_Fall,         ///< 落下
	PlayerAnimType_FallBig,      ///< 大げさに落下
	PlayerAnimType_RunStop,      ///< 歩いている最中に死後硬直
	PlayerAnimType_Sit,          ///< 座る
	PlayerAnimType_SitRest,      ///< ひじ掛けて座る
	PlayerAnimType_Die,          ///< 死亡！
	PlayerAnimType_Dig,          ///< 掘る
	PlayerAnimType_Yes,          ///< 首を縦に振る
	PlayerAnimType_No,           ///< 首を横に振る
	PlayerAnimType_RaiseLeft,    ///< 左手を上げる
	PlayerAnimType_RaiseRight,   ///< 右手を上げる
	PlayerAnimType_RaiseBoth,    ///< 両手を上げる
	PlayerAnimType_FireLeft,     ///< 左手で射撃
	PlayerAnimType_FireRight,    ///< 右手で射撃
	PlayerAnimType_FireBoth,     ///< 両手で射撃
	PlayerAnimType_StrikeLeft,   ///< 左手で殴る
	PlayerAnimType_StrikeRight,  ///< 右手で殴る
	PlayerAnimType_StrikeBoth,   ///< 両手で殴る
	PlayerAnimType_KickLeft,     ///< 左足で蹴る
	PlayerAnimType_KickRight,    ///< 右足で蹴る
	PlayerAnimType_PourLeft,     ///< 左手で注ぐ
	PlayerAnimType_PourRight,    ///< 右手で注ぐ
	PlayerAnimType_PourBoth,     ///< 両手で注ぐs
	                             // ...以下車いすのアニメーション...
};
