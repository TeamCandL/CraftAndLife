#include "pch\pch.h"
#include "YzMassSystem.h"

bool YzMassSystem::IsNowYzMass() const
{
	auto now = chr::utc_clock::now();

	auto todayYmd = chr::floor<chr::days>(chr::utc_clock::to_sys(now));
	chr::year y   = chr::year_month_day{ todayYmd }.year();

	return IsYzmassSeasonAnywhere(y, now);
}
