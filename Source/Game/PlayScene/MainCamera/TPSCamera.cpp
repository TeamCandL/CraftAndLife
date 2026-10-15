#include "pch\pch.h"
#include "TPSCamera.h"
#include "../MainCamera.h"
#include <Game/System/AbstructInputSystem.h>

void TPSCamera::Update(MainCameraData& _data)
{
	// _data.hogehogeが面倒なので参照して省略しまくる
	auto& [
		angles,
		position,
		moveSpeedPerSec,
		rotateSpeedPerSec,
		isHideCursor,
		objUpdater] = _data;

	// MEMO: これと同じこと
	//     : Vec2& angles = _data.angles;
	//     : Vec3& position = _data.position;
	//     : float& moveSpeedPerSec = _data.moveSpeedPerSec;
	//     : float& rotateSpeedPerSec = _data.rotateSpeedPerSec;
	//     : bool& isHideCursor = _data.isHideCursor;
	//     : GameObject& objUpdater = _data.objUpdater;

	// フレーム間時間を取得
	const float DELTA_TIME = Get<DeltaTime>();

	// カメラシステムのデータを参照
	CameraSystemData& camera = Get<CameraSystem>().Data();

	// 抽象入力システムから抽象プレイヤー入力を取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput = inputSystem.GetPlayerInput();

	// 回転スピード
	const float ROTATE_SPEED{ rotateSpeedPerSec * DELTA_TIME };
	// 移動スピード
	const float MOVE_SPEED{ moveSpeedPerSec * DELTA_TIME };

	// TODO: ここにTPSカメラの処理書く
}
