#pragma once
#include <Game/Game.h>
#include "IPlayerController.h"

/// @brief プレイヤーの普通の操作
struct PlayerControllerNormal : ObjectBase<>, IPlayerController
{
	/// @brief 更新処理
	void Update(PlayerData&) override;
};
