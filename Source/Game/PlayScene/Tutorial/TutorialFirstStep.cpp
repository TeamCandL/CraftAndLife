#include "pch\pch.h"
#include "TutorialFirstStep.h"
#include <Game/PlayScene/TutorialSystem.h>
#include <Game/System/AbstructInputSystem.h>
#include <Game/System/AbstructInputSystem/TutorialPlayerInput.h>
#include <Game/System/AbstructInputSystem/TutorialUIInput.h>
#include <Game/PlayScene/MainCamera.h>
#include <Game/PlayScene/PlaySceneUIController.h>
#include <Game/PlayScene/UI/TaskBackground.h>

void TutorialFirstStep::Start()
{
	TutorialSystem& tutorialSystem           = Get<TutorialSystem>();
	AbstructInputSystem& abstructInputSystem = Get<AbstructInputSystem>();

	tutorialSystem.ChangeObserverForce(this->ToGameObject());

	previousPlayerPosition = Get<Transform>(objPlayer).position;
	previousCameraAngles   = Get<MainCamera>(objMainCamera).GetAngle();

	objTaskBackground = Get<PlaySceneUIController>(objPlaySceneUIController).Data().objTaskBackground;
	Get<TaskBackground>(objTaskBackground).SetTask("右(→)スティックを倒して周りを見よう！");
}

bool TutorialFirstStep::CheckClearedTutorial(
    const TutorialElementType _type)
{
	switch (_type)
	{
	case TutorialElementType_CheckBodyMove:
		break;
	case TutorialElementType_CheckCameraMove:
		break;
	case TutorialElementType_CheckOpenBackpack:
		break;
	default:
		break;
	}

	return false;
}

GameObject TutorialFirstStep::GetNextObserverOnCleardElement(
    const TutorialElementFlags& _flags)
{
	return GameObject::Empty();
}

void TutorialFirstStep::SetTutorialInputEnable(
    const bool _enabled)
{
	if (_enabled)
	{
		TutorialSystem& tutorialSystem           = Get<TutorialSystem>();
		AbstructInputSystem& abstructInputSystem = Get<AbstructInputSystem>();

		Get<AbstructInputSystem>().SetPlayerInput(
		    std::make_shared<TutorialPlayerInput>(tutorialSystem, abstructInputSystem));
		Get<AbstructInputSystem>().SetUIInput(std::make_shared<TutorialUIInput>(tutorialSystem, abstructInputSystem));
	}
	else
	{
		Get<AbstructInputSystem>().ResetInput();
	}
}
