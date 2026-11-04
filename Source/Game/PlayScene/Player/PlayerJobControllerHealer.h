#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerHealerData
{
};

/// @brief ヒーラー固有の処理
struct PlayerJobControllerHealer : ObjectBase<PlayerJobControllerHealerData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;

	// IPlayerJobController を介して継承されました
	void Start(PlayerData& _data) override;
};
