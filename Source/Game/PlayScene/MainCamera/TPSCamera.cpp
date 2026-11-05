#include "pch\pch.h"
#include "TPSCamera.h"
#include "../MainCamera.h"
#include <Game/System/AbstructInputSystem.h>
#include "../DebugPoint.h"

void TPSCamera::Start()
{
	objDebugPoint = GetScene().Instantiate<DebugPoint>();
}

void TPSCamera::Update(
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
	float xRotateAssist{ angles.x * angles.x * xRotateAssistRatio + 1.0f };

	// カメラ移動をカメラの回転に適用
	angles.x += -pPlayerInput->GetMoveCamera().y * ROTATE_SPEED * xRotateAssist;
	angles.y += pPlayerInput->GetMoveCamera().x * ROTATE_SPEED;

	// 角度に制限をつける
	angles.x = std::clamp(angles.x, angleLimitXMin, angleLimitXMax);
#pragma endregion
}

void TPSCamera::UpdatePositionFromTarget(
    MainCameraData& _data,
    const GameObject _objTarget)
{
	// カメラシステムのデータを参照
	CameraSystemData& camera = Get<CameraSystem>().Data();

	Vec3 targetPosition = Get<Transform>(_objTarget).position;

	ImGui::Begin("TPS.UpdatePositionFromTarget");
	ImGui::DragFloat("xRotateAssistRatio", &xRotateAssistRatio, 0.1f);
	ImGui::DragFloat3("offsetFar", offsetFar.v, 0.1f);
	ImGui::DragFloat3("offsetNearLower", offsetNearLower.v, 0.1f);
	ImGui::DragFloat3("offsetNearUpper", offsetNearUpper.v, 0.1f);
	ImGui::DragFloat2("angles", _data.angles.v);

	Mat4x4 rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(_data.angles.x, _data.angles.y, 0.0f);

	// MEMO: 始点から終点までの線分と地面の平面との交点に移動する
	struct ClampGroundHeightResult
	{
		Vec3 p;
		float t;
	};
	auto ClampGroundHeight = [](const Vec3 p0,            ///< 視点座標
	                            const Vec3 p1,            ///< 終点座標
	                            const float groundHeight  ///< 地面の高さ
	                            ) -> ClampGroundHeightResult
	{
		// 最終的な点の位置
		Vec3 p = p1;
		float t{ 1.0f };

		// カメラが地面より下にあり、かつ中心点が地面以上の高さにある場合
		if (p1.y < groundHeight && p0.y >= groundHeight)
		{
			// 平面(Y = groundHeight)と線分の交点パラメータtを求める
			t = (groundHeight - p0.y) / (p1.y - p0.y);

			// 始点から終点に向かう直線上の交点を算出
			p = p0 + (p1 - p0) * t;
		}
		else if (p0.y < groundHeight)
		{
			p = p0;
			t = 0.0f;
		}

		return { .p = p, .t = t };
	};

	const float GROUND_HEIGHT{ targetPosition.y };

	// 注視点の終点
	Vec3 fp1 = DirectX::XMVector3TransformCoord(focusOffsetMax, rotationMatrix);
	fp1      = targetPosition + fp1;

	// 注視点の視点
	Vec3 fp0 = DirectX::XMVector3TransformCoord(focusOffsetMin, rotationMatrix);
	fp0      = targetPosition + fp0;

	auto [finalFocusPoint, focusPointT] = ClampGroundHeight(
	    fp0,
	    fp1,
	    targetPosition.y + /*targetPosition.y + std::min(focusOffsetMax.y, focusOffsetMin.y) - 0.1f*/ 0.5f);

	ImGui::DragFloat("focusPointT", &focusPointT);

	Vec3 localOffset =
	    DirectX::XMVector3TransformCoord(Mathf::Lerp(offsetNearLower, offsetFar, focusPointT), rotationMatrix);

	auto [finalCameraPosition, finalCameraT] = ClampGroundHeight(
	    targetPosition + axisOffset,  // 線分の始点
	    targetPosition + localOffset,
	    GROUND_HEIGHT);  // 本来のカメラワールド座標 終点

	ImGui::DragFloat("finalCameraT", &finalCameraT);

	if (finalCameraT < 1.0f)  // 上を向いたとき下の地面に当たるなら再計算する
	{
		localOffset =
		    DirectX::XMVector3TransformCoord(Mathf::Lerp(offsetNearUpper, offsetFar, finalCameraT), rotationMatrix);

		auto [p, t] = ClampGroundHeight(
		    targetPosition + axisOffset,  // 線分の始点
		    targetPosition + localOffset,
		    GROUND_HEIGHT);  // 本来のカメラワールド座標 終点

		finalCameraPosition = p;
	}

	// 最終的なカメラ位置をセット
	camera.transform.position = finalCameraPosition;
	_data.position            = finalCameraPosition;

	if (!objDebugPoint.IsEmpty())
	{
		Get<DebugPoint>(objDebugPoint).Data().transform.position = finalFocusPoint;
	}

	camera.transform.rotation =
	    Quaternion::FromLookAtQuaternion(camera.transform.position, finalFocusPoint, Vec3::Up());

	focusPosition = finalFocusPoint;

	ImGui::End();
}

void TPSCamera::OnDestroy()
{
	GetScene().Destroy(objDebugPoint);
}

Vec3 TPSCamera::GetFocusPosition()
{
	return focusPosition;
}
