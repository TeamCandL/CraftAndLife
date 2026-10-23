#include "pch\pch.h"
#include "PlaySceneUIController.h"
#include "UI/TaskBackground.h"

void PlaySceneUIController::Start()
{
	objTaskBackground = GetScene().Instantiate<TaskBackground>();
}
