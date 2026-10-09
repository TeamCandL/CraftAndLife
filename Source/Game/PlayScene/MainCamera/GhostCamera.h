#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

/// @brief 観戦者のカメラ
struct GhostCamera : ObjectBase<>, IMainCameraUpdater
{
	GhostCamera() = default;
	~GhostCamera() = default;

private:
	// IMainCameraUpdater を介して継承されました
	void Update(MainCameraData& _data) override;
};
