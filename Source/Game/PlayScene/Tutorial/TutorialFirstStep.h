#pragma once
#include <Game/Game.h>
#include "ITutorialObserver.h"
#include "../ItemId.h"

struct TutorialFirstStepData
{
	TutorialFirstStepData(
	    const GameObject _objPlaySceneUIController,
	    const GameObject _objPlayer,
	    const GameObject _objMainCamera) :
	    objPlaySceneUIController{ _objPlaySceneUIController },
	    objPlayer{ _objPlayer },
	    objMainCamera{ _objMainCamera }
	{
	}

	GameObject objTaskBackground{};
	GameObject objTutorialUI{};
	GameObject objPlayer{};
	GameObject objPlayerHand{};
	GameObject objMainCamera{};

	GameObject objPlaySceneUIController{};

	Vec3 previousPlayerPosition{};                              ///< 前回のプレイヤーの座標
	Vec3 totalPlayerMove{};                                     ///< 総プレイヤーの移動量
	const float COMPLITE_BODY_MOVE_DISTANCE_THRESHOLD{ 4.0f };  ///< 移動のチュートリアル完了とする総移動距離

	Vec2 previousCameraAngles{};  ///< 前回のカメラの角度
	Vec2 totalCameraAngles{};     ///< 総カメラの移動量
	const float COMPLITE_CAMERA_MOVE_ANGLES_THRESHOLD{
		DirectX::XM_PI
	};  ///< カメラのチュートリアル完了とする総移動角度

	const ItemId ITEM_ID_BACKPACK{ 19 };  ///< バックパックのアイテムId
};

struct TutorialFirstStep : ObjectBase<TutorialFirstStepData>, ITutorialObserver
{
	using ObjectBase<TutorialFirstStepData>::ObjectBase;

	void Start() override;

	void FillterCheckCleardElement(TutorialElementFlags& _flags, const TutorialFocusType _type) override;

	// ITutorialObserver を介して継承されました
	bool CheckClearedTutorial(const TutorialElementType _type) override;
	GameObject GetNextObserverOnCleardElement(
	    const TutorialElementType _cleardElement,
	    const TutorialElementFlags& _allFlags) override;

	/// @brief チュートリアル用の抽象入力を有効にするか
	/// @param _enabled 有効にする true / false 無効にする
	void SetTutorialInputEnable(const bool _enabled);
};
