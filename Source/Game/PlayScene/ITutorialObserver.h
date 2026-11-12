#pragma once
#include <Game/Game.h>
#include "TutorialPhaseType.h"

/// @brief チュートリアルチェック受信オブジェクトのインタフェース
class ITutorialObserver
{
  public:
	ITutorialObserver()          = default;
	virtual ~ITutorialObserver() = default;

	virtual bool CheckClearedTutorial(const TutorialPhaseType _type) = 0;

	virtual GameObject GetNextObserverOnCleardPhase(const TutorialPhaseFlags& _flags) = 0;
};
