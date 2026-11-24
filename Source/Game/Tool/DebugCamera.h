#pragma once
#include <Game/Game.h>

struct DebugCameraData
{
	Vec2 angles{ 0.248f, -2.356f };           ///< 現在のカメラの角度
	Vec3 position{ 2.919f, 2.081f, 3.083f };  ///< 現在のカメラの座標
	float moveSpeedPerSec{ 0.01f };           ///< カメラの移動スピード(/秒)
	float rotateSpeedPerSec{ 1.0f };          ///< カメラの回転スピード(/秒)
	GameObject objDebugPoint{};               ///< デバッグポイントのオブジェクト

	bool isEnabled{ false };  ///< 操作が有効である true / false
};

struct DebugCamera : ObjectBase<DebugCameraData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
