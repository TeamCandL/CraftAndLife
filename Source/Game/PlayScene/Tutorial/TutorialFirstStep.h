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

	Vec3 previousPlayerPosition{};
	Vec2 previousCameraAngles{};

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
