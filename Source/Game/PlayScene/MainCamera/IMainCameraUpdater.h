#pragma once
#include <Game/Game.h>

struct MainCameraData;

/// @brief メインカメラの更新処理のインタフェース
class IMainCameraUpdater
{
public:
	IMainCameraUpdater() = default;
	virtual ~IMainCameraUpdater() = default;

	/// @brief 更新処理
	/// @param _data データ
	virtual void Update(MainCameraData& _data) = 0;
};
