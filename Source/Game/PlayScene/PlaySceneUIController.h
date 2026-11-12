#pragma once
#include <Game/Game.h>

/// @brief プレイシーンのUI操作データ
struct PlaySceneUIControllerData
{
	PlaySceneUIControllerData(
	    GameObject objPlayer_,
	    GameObject objOtherPlayerController_,
	    GameObject objDayContloller_)
	{
		objDayContloller         = objDayContloller_;
		objPlayer                = objPlayer_;
		objOtherPlayerController = objOtherPlayerController_;
	};

	GameObject objDayContloller{};   ///< 日付の管理をしているオブジェクト
	GameObject objTaskBackground{};  ///< タスク情報の背景
	GameObject objTaskText{};        ///< タスク情報のテキスト
	GameObject objBossInfo{};        ///< ボスの体力ゲージUI
	GameObject objMiniMap{};         ///< ミニマップUI
	GameObject objOtherInfo{};       ///< 味方の体力ゲージ
	GameObject objSelfInfo{};        ///< 自分の体力ゲージ裏
	GameObject objMyGauge{};         ///< 自分の体力ゲージ緑
	GameObject objGauge1{};          ///< 他プレイヤー1ゲージ緑
	GameObject objGauge2{};          ///< 他プレイヤー2ゲージ緑
	GameObject objGauge3{};          ///< 他プレイヤー3ゲージ緑
	GameObject objGauge4{};          ///< 他プレイヤー4ゲージ緑
	GameObject objMyName{};          ///< 自分の名前
	GameObject objName1{};           ///< 仲間の名前上から１
	GameObject objName2{};           ///< 仲間の名前２
	GameObject objName3{};           ///< 仲間の名前３
	GameObject objName4{};           ///< 仲間の名前４
	GameObject objBossName{};        ///< ボスの名前
	GameObject objBossHp{};          ///< ボスのHP数値
	GameObject objBossGaugeR{};      ///< ボスのゲージ、赤
	GameObject objBossGauge{};       ///< ボスのゲージ、オレンジ
	GameObject objCraftUI{};         ///< クラフト画面
	GameObject objPlayerIcon2{};     ///< プレイヤー２ミニマップアイコン
	GameObject objPlayerIcon3{};     ///< プレイヤー３ミニマップアイコン
	GameObject objPlayerIcon4{};     ///< プレイヤー４ニマップアイコン

	GameObject objPlayer{};  ///< MyGaugeに渡すプレイヤー
	GameObject objOtherPlayerController{};

	// GameObject objEnemyInfo{};  ///< 敵の体力ゲージUI
	// GameObject objEnemyHp{};    ///< 敵のHP数値
	// GameObject objEnemyGaugeR{};  ///< 敵のゲージ、赤
	// GameObject objEnemyGaugeG{};   ///< 敵のゲージ、緑
};

/// @brief プレイシーンのUI操作
struct PlaySceneUIController : ObjectBase<PlaySceneUIControllerData>
{
	using ObjectBase<PlaySceneUIControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	void Update() override
	{
		// UIの更新処理は各UIオブジェクトに任せるため、ここでは特に何もしない
	}
};
