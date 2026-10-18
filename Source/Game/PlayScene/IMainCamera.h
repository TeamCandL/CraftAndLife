#pragma once
#include <Game/Game.h>

/// @brief メインカメラのインタフェース
class IMainCamera
{
public:
	IMainCamera() = default;
	virtual ~IMainCamera() = default;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _objTarget ターゲットのゲームオブジェクト
	virtual void UpdatePositionFromTarget(const GameObject _objTarget) = 0;
};
