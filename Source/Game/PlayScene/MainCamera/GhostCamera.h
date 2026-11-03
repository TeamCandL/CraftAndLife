#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

/// @brief 観戦者のカメラのデータ
struct GhostCameraData
{
	Vec3 focusPosition{};  ///< 注視している座標
};

/// @brief 観戦者のカメラ
struct GhostCamera : ObjectBase<GhostCameraData>, IMainCameraUpdater
{
	GhostCamera()  = default;
	~GhostCamera() = default;

	// IMainCameraUpdater を介して継承されました
	void Update(MainCameraData& _data) override;
	void UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget) override;

	// IMainCameraUpdater を介して継承されました
	Vec3 GetFocusPosition() override;
};
