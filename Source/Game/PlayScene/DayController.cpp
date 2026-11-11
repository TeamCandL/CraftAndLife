#include "pch\pch.h"
#include "DayController.h"
#include <Game/Common/Sky.h>
#include <Game/Manager/SessionManager.h>
#include <Game/Manager/AudioManager.h>

void DayController::Update()
{
#ifdef USE_IMGUI
	ImGui::Begin("DayController");
	ImGui::DragFloat("boost", &boost, 1.0f, 1.0f);
	ImGui::DragFloat("currentTimeSec", &currentTimeSec);
	ImGui::End();
#endif  // _DEBUG

	const float DELTA_TIME = Get<DeltaTime>();
	const float MAX_TIME   = skyAnimKeys.back().timeSec;

	// 前フレームの時刻を保存
	const float PREVIOUS_TIME_SEC = currentTimeSec;

	currentTimeSec += DELTA_TIME * boost;

	// ゲストだけホストの時刻に近づける
	const InSessionStatus& status = Get<SessionManager>().RefStatus();

	isDayChanged = false;
	if (status.imLobbyHost && currentTimeSec >= MAX_TIME)
	{
		isDayChanged = true;
		currentDay++;
	}

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

		// 時刻を0～180秒の範囲に収める
		currentTimeSec = std::fmodf(std::fmodf(currentTimeSec, MAX_TIME) + MAX_TIME, MAX_TIME);

		// ゲスト側も日付変更を検出
		if (!status.imLobbyHost && PREVIOUS_TIME_SEC > MAX_TIME * 0.5f && currentTimeSec < MAX_TIME * 0.5f)
		{
			currentDay++;
		}

		constexpr float CORRECTION_SPEED = 3.0f;

		currentTimeSec += diff * std::min(DELTA_TIME * CORRECTION_SPEED, 1.0f);
	}

	// 時刻を0～180秒の範囲に収める
	currentTimeSec = std::fmodf(std::fmodf(currentTimeSec, MAX_TIME) + MAX_TIME, MAX_TIME);

	// --------------------------------
	// 昼・夜の切り替わりSEと環境音
	// --------------------------------

	// 夜の時間帯は90秒～180秒
	bool nowNight = currentTimeSec >= 90.0f;

	AudioManager& audioManager = ManagerRegistry::Ref<AudioManager>();

	// 環境音の初回再生
	if (!isAmbientInitialized)
	{
		audioManager.SetAmbientSound(nowNight ? nighttimeAmbientSE : daytimeAmbientSE);

		isNight              = nowNight;
		isAmbientInitialized = true;
	}
	// 昼から夜に切り替わった
	else if (nowNight && !isNight)
	{
		// 夜への切り替わりSE
		audioManager.Play(nightfallSE);

		// 夜の環境音に変更
		audioManager.SetAmbientSound(nighttimeAmbientSE);

		isNight = true;
	}
	// 夜から昼に切り替わった
	else if (!nowNight && isNight)
	{
		// 朝への切り替わりSE
		audioManager.Play(morningSE);

		// 昼の環境音に変更
		audioManager.SetAmbientSound(daytimeAmbientSE);

		isNight = false;
	}

	// 現在の時間帯を更新
	isNight = nowNight;

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
