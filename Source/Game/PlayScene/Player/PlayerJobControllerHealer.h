#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerHealerData
{
	float downLookAngleThresholdRad{ DirectX::XM_PI / 180.0f *
		                             10.0f };  ///< ダウンしたプレイヤーを見ていると判定する角度(ラジアン)
};

/// @brief ヒーラー固有の処理
struct PlayerJobControllerHealer : ObjectBase<PlayerJobControllerHealerData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;

	// IPlayerJobController を介して継承されました
	void Start(PlayerData& _data) override;
};
