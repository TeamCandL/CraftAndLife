#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

struct TPSCameraData
{
	Vec3 offsetFar{ 0.7f, 2.0f, -5.0f };  ///< カメラが注視するターゲットの座標からのオフセット
	Vec3 offsetNearLower{ 0.5f, 1.4f, -0.8f };  ///< カメラが注視するターゲットの座標からのオフセット
	Vec3 offsetNearUpper{ 0.0f, 0.4f, 3.0f };  ///< カメラが注視するターゲットの座標からのオフセット
	Vec3 focusOffsetMax{ 0, 1, 10 };   ///< プレイヤーから直線上のに伸びる注視点の最長
	Vec3 focusOffsetMin{ 0.0f, 1.0f, 0.7f };    ///< プレイヤーから直線上のに伸びる注視点の最短

	float xRotateAssistRatio{ 2.0f };  ///< X軸回転の上下で速度アシストをするレート

	Vec3 axisOffset{ 0.0f, 1.5f, 0.0f };  ///< カメラ回転の軸 ターゲット座標からのオフセット

	float angleLimitXMin{ -DirectX::XM_PIDIV2 + 0.1f };  ///< カメラのX角度最小
	float angleLimitXMax{ 0.714f };   ///< カメラのX角度最大

	GameObject objDebugPoint{};
};

/// @brief 3人称カメラ
struct TPSCamera : ObjectBase<TPSCameraData>, IMainCameraUpdater
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update(MainCameraData& _data) override;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _objTarget ターゲットのゲームオブジェクト
	void UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget) override;

	/// @brief オブジェクトが消されたときの処理
	void OnDestroy() override;
};
