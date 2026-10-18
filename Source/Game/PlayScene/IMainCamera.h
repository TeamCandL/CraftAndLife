#pragma once
#include <Game/Game.h>

/// @brief メインカメラのインタフェース
class IMainCamera
{
public:
	/// @brief コンストラクタ
	IMainCamera() = default;
	/// @brief デストラクタ
	virtual ~IMainCamera() = default;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _objTarget ターゲットのゲームオブジェクト
	virtual void UpdatePositionFromTarget(const GameObject _objTarget) = 0;

	/// @brief カメラの角度xを取得する
	/// @return ラジアン角度
	virtual float GetAngleX() const = 0;
	/// @brief カメラの角度yを取得する
	/// @return ラジアン角度 0.0f <= v < 2PI
	virtual float GetAngleY() const = 0;
};
