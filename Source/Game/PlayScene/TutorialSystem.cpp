#include "pch\pch.h"
#include "TutorialSystem.h"
#include "ITutorialObserver.h"

void TutorialSystem::Start() {}

void TutorialSystem::Update()
{
	if (objObserver.IsEmpty())
	{
		return;  // ターゲットのオブザーバーがないなら無視
	}

	for (TutorialPhaseType type{}; type < TutorialPhaseType_Max; type++)
	{
		if (!phaseClearFlags[type])
		{
			ITutorialObserver& observer = Get<ITutorialObserver>(objObserver);
			phaseClearFlags[type]       = observer.CheckClearedTutorial(type);
			if (phaseClearFlags[type])
			{
				GameObject objNextObserver = observer.GetNextObserverOnCleardPhase(phaseClearFlags);
				if (!objNextObserver.IsEmpty())
				{
					objObserver = objNextObserver;
					return;
				}
			}
		}
	}
}

void TutorialSystem::ChangeObserverForce(
    const GameObject _obj)
{
	objObserver = _obj;
}
