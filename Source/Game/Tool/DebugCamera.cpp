#include "pch\pch.h"
#include "DebugCamera.h"
#include <Game/System/AbstructInputSystem.h>
#include <../Game/PlayScene/DebugPoint.h>
#include <Game/PlayScene/TerrainSystem.h>

void DebugCamera::Start()
{
	// 地形上に表示するデバッグポイントを生成
	objDebugPoint = GetScene().Instantiate<DebugPoint>();

	// カメラシステムのデータを参照
	CameraSystemData& camera = Get<CameraSystem>().Data();

	// 回転をtransformに適用
	camera.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(angles.x, angles.y, 0.0f);

	// 座標をtransformに適用
	camera.transform.position = position;
}

void DebugCamera::Update()
{
	ImGui::Begin("DebugCamera");
	if (ImGui::Button(isEnabled ? "無効化する" : "有効化する"))
	{
		isEnabled = !isEnabled;
	}
	ImGui::DragFloat3("position", position.v);
	ImGui::DragFloat2("angles", angles.v);
	ImGui::End();

	if (!isEnabled)
	{
		return;  // 操作無効なら無視
	}

	// フレーム間時間を取得
	const float DELTA_TIME = Get<DeltaTime>();

	// カメラシステムのデータを参照
	CameraSystemData& camera = Get<CameraSystem>().Data();

	AbstructInputSystem& input = Get<AbstructInputSystem>();
	auto pPlayerInput          = input.GetPlayerInput();

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
#pragma endregion

	float y                                                  = Get<TerrainSystem>().GetHeight(position.x, position.z);
	Get<DebugPoint>(objDebugPoint).Data().transform.position = { position.x, y, position.z };
}
