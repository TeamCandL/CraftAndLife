#pragma once
#include <Game/Game.h>
#include "TutorialPhaseType.h"

/// @brief チュートリアルチェック受信オブジェクトのインタフェース
class ITutorialObserver
{
  public:
	ITutorialObserver()          = default;
	virtual ~ITutorialObserver() = default;

	virtual bool CheckClearedTutorial(const TutorialElementType _type) = 0;

	virtual GameObject GetNextObserverOnCleardElement(const TutorialElementFlags& _flags) = 0;
};
