#pragma once
#include <Game/Game.h>

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

struct TPSCamera : ObjectBase<TPSCameraData>
{

};