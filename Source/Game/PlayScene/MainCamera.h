#pragma once
#include <Game/Game.h>
#include "MainCamera/IMainCameraUpdater.h"
#include "IMainCamera.h"

/// @brief メインのカメラデータ
struct MainCameraData
{
	Vec2 angles{ 0.0, 0.7 };               ///< 現在のカメラの角度
	Vec3 position{};                       ///< 現在のカメラの座標
	float moveSpeedPerSec{ 0.01f };        ///< カメラの移動スピード(/秒)
	Vec2 rotateSpeedPerSec{ 3.0f, 5.0f };  ///< カメラの回転スピード(/秒)
	bool isHideCursor{ false };            ///< カーソルを隠すか
	bool isActiveControl{};                ///< 操作可能か true / false

	GameObject objUpdater{};  ///< カメラの更新処理ステート用オブジェクト
};

/// @brief メインのカメラ
struct MainCamera : ObjectBase<MainCameraData>, IMainCamera
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void UpdateLocal();

	/// @brief カメラの座標を設定する
	/// @param _worldPosition ワールド座標
	void SetPosition(const Vec3 _worldPosition);

	/// @brief カメラの更新モードを変更する
	/// @param _objUpdater 新しく設定する更新モード
	void ChangeUpdater(const GameObject _objUpdater);

	/// @brief カメラの種類を設定する
	/// @param _type カメラの種類
	void SetCameraType(const MainCameraType _type) override;

	/// @brief 注視している座標を取得する
	/// @return 注視している座標 xyz
	Vec3 GetFocusPosition();

	/// @brief カメラの座標を取得する
	/// @return カメラの座標 xyz
	Vec3 GetPosition();

	// IMainCamera を介して継承されました
	void UpdatePositionFromTarget(const GameObject _objTarget) override;
	float GetAngleX() const override;
	float GetAngleY() const override;
	Vec2 GetAngle() const
	{
		return angles;
	}

	void SetActiveControl(
	    const bool _isActiveControl) override
	{
		isActiveControl = _isActiveControl;
	}
};
