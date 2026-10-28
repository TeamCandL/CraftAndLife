#pragma once
#include "IPlayerJobController.h"

/// @brief ファイター固有の処理
struct PlayerJobControllerFighter : IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;
};
