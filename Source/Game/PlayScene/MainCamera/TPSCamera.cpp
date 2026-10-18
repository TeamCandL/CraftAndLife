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

#pragma region カメラの回転処理
	// カメラ移動をカメラの回転に適用
	angles.x += -pPlayerInput->GetMoveCamera().y * ROTATE_SPEED;
	angles.y += pPlayerInput->GetMoveCamera().x * ROTATE_SPEED;

	// 角度に制限をつける
	angles.x = std::clamp(
		angles.x,
		angleLimitXMin,
		angleLimitXMax);
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

	Mat4x4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
		_data.angles.x,
		_data.angles.y,
		0.0f);

	Vec3 localOffset = DirectX::XMVector3TransformCoord(offset, rotationMatrix);

	// MEMO: 始点から終点までの線分と地面の平面との交点に移動する

	// 本来のカメラワールド座標 終点
	Vec3 p1 = targetPosition + localOffset;
	// 線分の始点
	Vec3 p0 = targetPosition + axisOffset;

	const float groundHeight{ 0.0f };
	Vec3 finalCameraPosition = p1;

	// カメラが地面より下にあり、かつ中心点が地面以上の高さにある場合
	if (p1.y < groundHeight && p0.y >= groundHeight)
	{
		// 平面(Y = groundHeight)と線分の交点パラメータtを求める
		float t = (groundHeight - p0.y) / (p1.y - p0.y);

		// 始点から終点に向かう直線上の交点を算出
		finalCameraPosition = p0 + (p1 - p0) * t;
	}
	else if (p1.y < groundHeight)
	{
		// 始点自体が地面より下にいる場合はカメラを真上に持っていく
		finalCameraPosition.y = groundHeight;
	}

	// 最終的なカメラ位置をセット
	camera.transform.position = finalCameraPosition;

	Vec3 focusOffsetWorld = DirectX::XMVector3TransformCoord(
		focusOffsetMax,
		rotationMatrix);

	Vec3 focusPoint = targetPosition + focusOffsetWorld;

	camera.transform.rotation = Quaternion::FromLookAtQuaternion(
		camera.transform.position,
		focusPoint,
		Vec3::Up());
}
