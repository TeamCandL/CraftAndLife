#pragma once
#include <Game/Game.h>
#include "IMainCameraUpdater.h"

struct TPSCameraData
{
	/// @brief コンストラクタ
	/// @param _objTarget ターゲットとなるオブジェクト
	TPSCameraData(const GameObject _objTarget) :
		objTarget{ _objTarget }
	{
	}

	GameObject objTarget{};  ///< ターゲットのオブジェクト
	//Vec3 targetOffset{ 1, 1, 0 };     ///< カメラが注視するターゲットの座標からのオフセット

};

/// @brief 3人称カメラ
struct TPSCamera : ObjectBase<TPSCameraData>, IMainCameraUpdater
{
	using ObjectBase<TPSCameraData>::ObjectBase;
	
	/// @brief 更新処理
	void Update(MainCameraData& _data) override;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _objTarget ターゲットのゲームオブジェクト
	void UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget) override;
};
