#pragma once
#include "IPlayerJobController.h"

struct PlayerJobControllerArcherData
{
	Vec3 dodgeVelocity{};     ///< 回避アクション時の速度
	float friction{ 0.09f };  ///< 回避アクションの速度減衰
};

/// @brief アーチャー固有の処理
struct PlayerJobControllerArcher : ObjectBase<PlayerJobControllerArcherData>, IPlayerJobController
{
	// IPlayerJobController を介して継承されました
	void Update(PlayerData& _data) override;

	// IPlayerJobController を介して継承されました
	void Start(PlayerData& _data) override;
};
