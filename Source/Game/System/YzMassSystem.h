#pragma once
#include <Game/Game.h>

#include <chrono>
#include <iostream>

namespace
{
	namespace chr = std::chrono;

	bool IsYzmassSeasonAnywhere(
	    chr::year _y,
	    chr::utc_clock::time_point _nowUtc)
	{
		auto start_sys = chr::sys_days{ _y / chr::December / 23 } - chr::hours{ 14 };

		auto end_sys = chr::sys_days{ _y / chr::December / 26 } + chr::hours{ 12 };

		auto start_utc = chr::utc_clock::from_sys(start_sys);
		auto end_utc   = chr::utc_clock::from_sys(end_sys);

		return (_nowUtc >= start_utc) && (_nowUtc < end_utc);
	}
}  // namespace

struct YzMassSystem : SystemObjectBase<YzMassSystem>
{
	bool IsNowYzMass() const;
};
