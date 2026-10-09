#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

/// @brief FPSカメラのデータ
struct FPSCameraData
{

};

/// @brief FPSカメラ
struct FPSCamera : ObjectBase<FPSCameraData>, IMainCameraUpdater
{
	// IMainCameraUpdater を介して継承されました
	void Update(MainCameraData& _data) override;
};
