#pragma once
#include <Game/Game.h>
#include "../TutorialPhaseType.h"

/// @brief チュートリアルチェック受信オブジェクトのインタフェース
class ITutorialObserver
{
  public:
	ITutorialObserver()          = default;
	virtual ~ITutorialObserver() = default;

	virtual void FillterCheckCleardElement(TutorialElementFlags& _flags, const TutorialFocusType _type) = 0;

	virtual bool CheckClearedTutorial(const TutorialElementType _type) = 0;

	virtual GameObject GetNextObserverOnCleardElement(
	    const TutorialElementType _cleardElement,
	    const TutorialElementFlags& _allFlags) = 0;
};
