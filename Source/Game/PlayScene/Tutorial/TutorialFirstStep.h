#pragma once
#include <Game/Game.h>
#include "ITutorialObserver.h"

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
	GameObject objPlayer{};
	GameObject objMainCamera{};

	GameObject objPlaySceneUIController{};

	Vec3 previousPlayerPosition{};
	Vec2 previousCameraAngles{};
};

struct TutorialFirstStep : ObjectBase<TutorialFirstStepData>, ITutorialObserver
{
	using ObjectBase<TutorialFirstStepData>::ObjectBase;

	void Start() override;

	// ITutorialObserver を介して継承されました
	bool CheckClearedTutorial(const TutorialElementType _type) override;
	GameObject GetNextObserverOnCleardElement(const TutorialElementFlags& _flags) override;

	/// @brief チュートリアル用の抽象入力を有効にするか
	/// @param _enabled 有効にする true / false 無効にする
	void SetTutorialInputEnable(const bool _enabled);
};
