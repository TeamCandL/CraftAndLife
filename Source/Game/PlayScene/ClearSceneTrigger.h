#pragma once
#include <Game/Game.h>

struct ClearSceneTriggerData
{
	ClearSceneTriggerData(
	    GameObject _objDayController,
	    GameObject _objClearFade) :
	    objDayController{ _objDayController },
	    objClearFade{ _objClearFade }
	{
	}

	GameObject objDayController{};
	GameObject objClearFade{};
};

struct ClearSceneTrigger : ObjectBase<ClearSceneTriggerData>
{
	using ObjectBase<ClearSceneTriggerData>::ObjectBase;

	void Update() override;
};