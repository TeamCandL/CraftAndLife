#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerTankData
{
};

/// @brief タンク固有の処理
struct PlayerJobControllerTank : ObjectBase<PlayerJobControllerTankData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;
};
