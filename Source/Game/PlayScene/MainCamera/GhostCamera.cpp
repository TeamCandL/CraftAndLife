#include "pch\pch.h"
#include "GhostCamera.h"
#include "../MainCamera.h"
#include <Game/System/AbstructInputSystem.h>

void GhostCamera::Update(
    MainCameraData& _data)
{
	// _data.hogehogeが面倒なので参照して省略しまくる
	auto& [angles, position, moveSpeedPerSec, rotateSpeedPerSec, isHideCursor, objUpdater] = _data;

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
	auto pPlayerInput                = inputSystem.GetPlayerInput();

	// 回転スピード
	const float ROTATE_SPEED{ rotateSpeedPerSec * DELTA_TIME };
	// 移動スピード
	const float MOVE_SPEED{ moveSpeedPerSec * DELTA_TIME };

#pragma region カメラの回転処理
	// カメラ移動をカメラの回転に適用
	angles.x += -pPlayerInput->GetMoveCamera().y * ROTATE_SPEED;
	angles.y += pPlayerInput->GetMoveCamera().x * ROTATE_SPEED;

	// 角度に制限をつける
	angles.x = std::clamp(angles.x, -DirectX::XM_PIDIV2 + 0.01f, DirectX::XM_PIDIV2 - 0.01f);

	// 回転をtransformに適用
	camera.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, 0.0f);
#pragma endregion

#pragma region カメラの移動処理
	Vec3 move{};

	// 前後方向の移動
	move += camera.transform.Forward() * pPlayerInput->GetMoveBody().y;
	// 左右方向の移動
	move += camera.transform.Right() * pPlayerInput->GetMoveBody().x;
	// 左右トリガーで上下移動
	move += camera.transform.Up() * (pPlayerInput->GetActionTrigger() + -pPlayerInput->GetSubActionTrigger());

	// ジャンプボタンが押されているならスピードを上げる
	if (!pPlayerInput->IsJump())
	{
		move *= 1000.0f;
	}

	// 移動を時間調整する適用
	move *= MOVE_SPEED;

	// 移動を適用
	position += move;

	// 移動した座標をtransformに適用
	camera.transform.position = position;

	// 注視している座標も更新
	focusPosition = camera.transform.Forward();

#pragma endregion
}

void GhostCamera::UpdatePositionFromTarget(
    MainCameraData& _data,
    const GameObject _objTarget)
{
	// なにもしないでおく
	//_data.position = Get<Transform>(_objTarget).position;
}

Vec3 GhostCamera::GetFocusPosition()
{
	return focusPosition;
}
