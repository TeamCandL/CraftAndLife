#include "pch\pch.h"
#include "TaskBackground.h"
#include <Game/Manager/SessionManager.h>
#include "../DayController.h"

void TaskBackground::Start()
{
	text.content = "-----";
}

void TaskBackground::Update()
{
	if (!isEnabledDayTimer)
	{
		return;  // 残り時間と日付の表示が有効ではないなら無視！
	}

	int currentDay = Get<DayController>(objDayContloller).GetCurrentDay();

	// Dayが3のときだけ、ダウンタイマーを表示する
	if (currentDay == 3)
	{
		float limitTime     = 47.0f;
		float currentTime   = Get<DayController>(objDayContloller).GetCurrentTimeSec();
		float remainingTime = limitTime - currentTime;

		// マイナスにならないように止める
		if (remainingTime < 0.0f)
		{
			remainingTime = 0.0f;
		}

		text.content = std::format("Day: {} / Rest: {:05.2f}", currentDay, remainingTime);
	}
	else
	{
		// 1日目や2日目は秒数を表示しない
		text.content = std::format("Day: {}", currentDay);
	}
}

void TaskBackground::SetTask(
    const std::string& _taskContent)
{
	text.content = _taskContent;
}
