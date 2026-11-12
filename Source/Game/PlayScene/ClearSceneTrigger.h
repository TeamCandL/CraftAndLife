#pragma once
#include <Game/Game.h>

struct ClearSceneTriggerData
{
	ClearSceneTriggerData(
	    GameObject _objDayController,
	    GameObject _objClearFade,
	    GameObject _objNetworkActorServer) :
	    objDayController{ _objDayController },
	    objClearFade{ _objClearFade },
	    objNetworkActorServer{ _objNetworkActorServer }
	{
	}

	GameObject objDayController{};
	GameObject objClearFade{};
	GameObject objNetworkActorServer{};
};

struct ClearSceneTrigger : ObjectBase<ClearSceneTriggerData>
{
	using ObjectBase<ClearSceneTriggerData>::ObjectBase;

	void Update() override;
};