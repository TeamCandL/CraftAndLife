#pragma once
#include <Game/Game.h>

struct MainCameraData;

/// @brief メインカメラの更新処理のインタフェース
class IMainCameraUpdater
{
  public:
	IMainCameraUpdater()          = default;
	virtual ~IMainCameraUpdater() = default;

	/// @brief 更新処理
	/// @param _data データ
	virtual void Update(MainCameraData& _data) = 0;

	/// @brief カメラの座標をターゲットから更新する
	/// @param _data データ
	/// @param _objTarget ターゲットのゲームオブジェクト
	virtual void UpdatePositionFromTarget(MainCameraData& _data, const GameObject _objTarget) = 0;

	/// @brief 注視している座標を取得する
	/// @return 注視している座標
	virtual Vec3 GetFocusPosition() = 0;
};
