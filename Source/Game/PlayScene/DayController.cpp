#include "pch\pch.h"
#include "DayController.h"
#include <Game/Common/Sky.h>
#include <Game/Manager/SessionManager.h>

void DayController::Update()
{
	ImGui::Begin("DayController");
	ImGui::DragFloat("boost", &boost, 1.0f, 1.0f);
	ImGui::DragFloat("currentTimeSec", &currentTimeSec);
	ImGui::End();

	const float DELTA_TIME = Get<DeltaTime>();
	const float MAX_TIME   = skyAnimKeys.back().timeSec;

	currentTimeSec += DELTA_TIME * boost;

	// ゲストだけホストの時刻に近づける
	const InSessionStatus& status = Get<SessionManager>().RefStatus();

	if (!status.imLobbyHost && status.hasReceivedDayTime)
	{
		float diff = status.currentTimeSec - currentTimeSec;

		// 180秒から0秒に戻る場合の補正
		if (diff > MAX_TIME * 0.5f)
		{
			diff -= MAX_TIME;
		}
		else if (diff < -MAX_TIME * 0.5f)
		{
			diff += MAX_TIME;
		}

		constexpr float CORRECTION_SPEED = 3.0f;

		currentTimeSec += diff * std::min(DELTA_TIME * CORRECTION_SPEED, 1.0f);
	}

	// 時刻を0～180秒の範囲に収める
	currentTimeSec = std::fmodf(std::fmodf(currentTimeSec, MAX_TIME) + MAX_TIME, MAX_TIME);

	if (currentTimeSec <= FLT_EPSILON)
	{
		Get<Sky>(objSky).Data().skyTime = skyAnimKeys.front().skyParam;
		return;
	}

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

	// 先頭要素の場合はitr--しない
	if (itr == skyAnimKeys.begin())
	{
		Get<Sky>(objSky).Data().skyTime = itr->skyParam;
		return;
	}

	SkyAnimKey& end = *itr;
	itr--;
	SkyAnimKey& begin = *itr;

	const float RATIO = (currentTimeSec - begin.timeSec) / (end.timeSec - begin.timeSec);

	Get<Sky>(objSky).Data().skyTime = Float4::Lerp(begin.skyParam, end.skyParam, RATIO);
}
