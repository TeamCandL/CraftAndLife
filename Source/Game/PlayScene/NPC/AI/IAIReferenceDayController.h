#pragma once
#include <Game/Game.h>

/// @brief AIからの参照用インタフェース：DayController
class IAIReferenceDayController
{
  public:
	IAIReferenceDayController()          = default;
	virtual ~IAIReferenceDayController() = default;

	/// @brief 今が昼であるかを取得
	/// @return 昼である true / false
	virtual bool IsDaytime() const = 0;
};
