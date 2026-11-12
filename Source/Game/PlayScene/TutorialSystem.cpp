#include "pch\pch.h"
#include "TutorialSystem.h"
#include "Tutorial/ITutorialObserver.h"

void TutorialSystem::Start() {}

void TutorialSystem::Update()
{
	if (objObserver.IsEmpty())
	{
		return;  // ターゲットのオブザーバーがないなら無視
	}

	for (TutorialElementType type{}; type < TutorialElementType_Max; type++)
	{
		if (!elementClearFlags[type])
		{
			ITutorialObserver& observer = Get<ITutorialObserver>(objObserver);
			elementClearFlags[type]     = observer.CheckClearedTutorial(type);
			if (elementClearFlags[type])
			{
				GameObject objNextObserver = observer.GetNextObserverOnCleardElement(elementClearFlags);
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
