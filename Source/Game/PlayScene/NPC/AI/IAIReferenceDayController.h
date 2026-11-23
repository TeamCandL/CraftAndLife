#pragma once
#include <Game/Game.h>

/// @brief AIからの参照用インタフェース：DayController
class IAIReferenceDayController
{
  public:
	IAIReferenceDayController()          = default;
	virtual ~IAIReferenceDayController() = default;

	virtual bool IsDaytime() const = 0;
};
