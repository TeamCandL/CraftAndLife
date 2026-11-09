#include "ClearSceneTrigger.h"
#include <Game/ClearScene/ClearScene.h>
#include "DayController.h"
#include "ClearFade.h"

void ClearSceneTrigger::Update()
{
	// フェード中なら何もしない
	ClearFade& fade = Get<ClearFade>(Data().objClearFade);

	if (Get<InputSystem>().GetKeyDown(KeyCode::Enter))
	{
		fade.StartFade();
		return;
	}

	// 現在の日数と時刻を取得
	DayController& dayController = Get<DayController>(Data().objDayController);

	const int32_t CURRENT_DAY = dayController.GetCurrentDay();
	const float CURRENT_TIME  = dayController.GetCurrentTimeSec();

	// 3日目の昼45秒を経過したらフェード開始
	if (CURRENT_DAY >= 3 && CURRENT_TIME >= 45.0f && CURRENT_TIME < 90.0f)
	{
		fade.StartFade();
	}
}