#pragma once
#include <Game/Game.h>

/// @brief プレイシーンのUI操作データ
struct PlaySceneUIControllerData
{
	PlaySceneUIControllerData(
	    GameObject objPlayer_)
	{
		objPlayer = objPlayer_;
	};

	GameObject objTaskBackground{};  ///< タスク情報の背景
	GameObject objTaskText{};        ///< タスク情報のテキスト
	GameObject objBossInfo{};        ///< ボスの体力ゲージUI
	GameObject objHotBar{};          ///< アイテムのホットスロットバーUI
	GameObject objMiniMap{};         ///< ミニマップUI
	GameObject objOtherInfo{};       ///< 味方の体力ゲージ
	GameObject objSelfInfo{};        ///< 自分の体力ゲージ裏
	GameObject objMyGage{};          ///< 自分の体力ゲージ緑

	GameObject objPlayer{};  ///< MyGageに渡すプレイヤー
};

/// @brief プレイシーンのUI操作
struct PlaySceneUIController : ObjectBase<PlaySceneUIControllerData>
{
	using ObjectBase<PlaySceneUIControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
};
