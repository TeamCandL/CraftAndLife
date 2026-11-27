#pragma once
#include <Game/Game.h>

using MainCameraType = int32_t;
enum MainCameraType_
{
	MainCameraType_TPS,    ///< TPSカメラ
	MainCameraType_Ghost,  ///< 観戦カメラ
};

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

	/// @brief カメラの種類を設定する
	/// @param _type カメラの種類
	virtual void SetCameraType(const MainCameraType _type) = 0;

	/// @brief 操作可能かをセットする
	/// @param _isActiveControl 操作可能 true / false
	virtual void SetActiveControl(const bool _isActiveControl) = 0;
};
