#pragma once
#include <Game/Game.h>

/// @brief アニメーションのシステムのデータ
struct AnimationSystemData
{
};

/// @brief アニメーションのシステム
struct AnimationSystem : SystemObjectBase<AnimationSystem, AnimationSystemData>
{
	/// @brief 更新処理
	void Update() override;
};
