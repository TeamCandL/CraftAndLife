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
#pragma region カメラの回転処理
	// カメラ移動をカメラの回転に適用
	angles.x += -pPlayerInput->GetMoveCamera().y * ROTATE_SPEED;
	angles.y += pPlayerInput->GetMoveCamera().x * ROTATE_SPEED;

	// 角度に制限をつける
	angles.x = std::clamp(
		angles.x,
		-DirectX::XM_PIDIV2 + 0.01f,
		DirectX::XM_PIDIV2 - 0.01f);
#pragma endregion
}

void TPSCamera::UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget)
{
	// カメラシステムのデータを参照
	CameraSystemData& camera = Get<CameraSystem>().Data();

	Vec3 targetPosition = Get<Transform>(_objTarget).position;
	
	ImGui::Begin("TPS.UpdatePositionFromTarget");
	ImGui::DragFloat3("offset", offset.v, 0.1f);
	ImGui::DragFloat2("angles", _data.angles.v);
	ImGui::End();

	// 回転をtransformに適用
	/*camera.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(
		angles.x,
		angles.y,
		0.0f);*/

	//camera.transform.UpdateSelf();

	Mat4x4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
		_data.angles.x,
		_data.angles.y,
		0.0f);

	Vec3 localOffset = DirectX::XMVector3TransformCoord(offset, rotationMatrix);

	camera.transform.position = targetPosition + localOffset;

	Vec3 focusOffsetWorld = DirectX::XMVector3TransformCoord(
		focusOffsetMax,
		rotationMatrix);

	Vec3 focusPoint = targetPosition + focusOffsetWorld;

	camera.transform.rotation = Quaternion::FromLookAtQuaternion(
		camera.transform.position,
		focusPoint,
		Vec3::Up());
}
