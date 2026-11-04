#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerArcherData
{
};

/// @brief アーチャー固有の処理
struct PlayerJobControllerArcher : ObjectBase<PlayerJobControllerArcherData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;

	// IPlayerJobController を介して継承されました
	void Start(PlayerData& _data) override;
};
