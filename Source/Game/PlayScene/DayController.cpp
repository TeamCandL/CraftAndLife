#include "pch\pch.h"
#include "DayController.h"
#include <Game/Common/Sky.h>

void DayController::Update()
{
	ImGui::Begin("DayController");
	ImGui::DragFloat("boost", &boost, 1.0f, 1.0f);
	ImGui::DragFloat("currentTimeSec", &currentTimeSec);
	ImGui::End();

	const float DELTA_TIME = Get<DeltaTime>();
	currentTimeSec += DELTA_TIME * boost;

	if (currentTimeSec <= FLT_EPSILON || boost <= FLT_EPSILON)
	{
		return;  // 0 なら無視
	}

	currentTimeSec = std::fmodf(currentTimeSec, skyAnimKeys.back().timeSec);

	auto itr = std::find_if(
	    skyAnimKeys.begin(),
	    skyAnimKeys.end(),
	    [this](const SkyAnimKey& _key)
	    {
		    return currentTimeSec <= _key.timeSec;
	    });

	if (itr == skyAnimKeys.end())
	{
		return;
	}

	SkyAnimKey& end = *itr;
	itr--;
	SkyAnimKey& begin = *itr;

	const float RATIO               = (currentTimeSec - begin.timeSec) / (end.timeSec - begin.timeSec);
	Get<Sky>(objSky).Data().skyTime = Float4::Lerp(begin.skyParam, end.skyParam, RATIO);
}
