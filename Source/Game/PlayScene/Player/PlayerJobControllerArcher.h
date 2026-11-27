#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerArcherData
{
	float friction{ 0.07f };  ///< 回避アクションの速度減衰
	float dashForce{ 0.1f };  ///< ダッシュパワー

	// ダッシュSE
	Resource<Audio> dashSE{ "BGM_SE/Dash.mp3" };
};

/// @brief アーチャー固有の処理
struct PlayerJobControllerArcher : ObjectBase<PlayerJobControllerArcherData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;

	// IPlayerJobController を介して継承されました
	void Start(PlayerData& _data) override;
};