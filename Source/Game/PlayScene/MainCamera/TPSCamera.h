#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

struct TPSCameraData
{
	Vec3 offset{ 1, 2, -10 };       ///< カメラが注視するターゲットの座標からのオフセット
	Vec3 focusOffsetMax{ 0, 1, 100 };  ///< プレイヤーから直線上のに伸びる注視点の最長
	Vec3 focusOffsetMin{ 0, 1, 1 };    ///< プレイヤーから直線上のに伸びる注視点の最短

	Vec3 axisOffset{ 0.0f, 1.5f, 0.0f };  ///< カメラ回転の軸 ターゲット座標からのオフセット

	float angleLimitXMin{ -DirectX::XM_PIDIV2 + 0.1f };  ///< カメラのX角度最小
	float angleLimitXMax{ DirectX::XM_PIDIV2 - 0.1f };   ///< カメラのX角度最大
};

/// @brief 3人称カメラ
struct TPSCamera : ObjectBase<TPSCameraData>, IMainCameraUpdater
{
	/// @brief 更新処理
	void Update(MainCameraData& _data) override;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _objTarget ターゲットのゲームオブジェクト
	void UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget) override;
};
