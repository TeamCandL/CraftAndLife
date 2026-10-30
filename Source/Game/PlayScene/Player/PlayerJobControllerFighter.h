#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerFighterData
{
};

/// @brief ファイター固有の処理
struct PlayerJobControllerFighter : ObjectBase<PlayerJobControllerFighterData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;
};
