#pragma once
#include <Game/Game.h>

struct DayControllerData
{
	struct SkyAnimKey
	{
		float timeSec{};
		Float4 skyParam{};
	};

	using SkyParamType = uint32_t;
	enum SkyParamType_ : SkyParamType
	{
		SkyParamType_MidNight,
		SkyParamType_MidDay,
		SkyParamType_Sunset,
		SkyParamType_Sunrise,
		SkyParamType_Max,
	};

	DayControllerData(
	    const GameObject _objSky) :
	    objSky{ _objSky }
	{
	}
	const std::array<Float4, SkyParamType_Max> SKY_PARAMS{
		Float4{ 1.0f, 0.44f, 0.0f, 0.0f },
		Float4{ 0.0f, 1.0f, 0.0f, 0.0f },
		Float4{ 0.55f, 0.0f, 0.61f, 0.34f },
		Float4{ 0.45f, 0.0f, 0.3f, 0.08f },
	};
	std::vector<SkyAnimKey> skyAnimKeys{
		{ 0.0f, SKY_PARAMS[SkyParamType_MidDay] },     { 80.0f, SKY_PARAMS[SkyParamType_MidDay] },
		{ 85.0f, SKY_PARAMS[SkyParamType_Sunset] },    { 90.0f, SKY_PARAMS[SkyParamType_MidNight] },
		{ 170.0f, SKY_PARAMS[SkyParamType_MidNight] }, { 175.0f, SKY_PARAMS[SkyParamType_Sunrise] },
		{ 180.0f, SKY_PARAMS[SkyParamType_MidDay] },
	};

	float boost{ 1.0f };
	float currentTimeSec{};
	GameObject objSky{};
};

struct DayController : ObjectBase<DayControllerData>
{
	using ObjectBase<DayControllerData>::ObjectBase;

	void Update() override;

	float GetCurrentTimeSec() const
	{
		return Data().currentTimeSec;
	}

	void SetCurrentTimeSec(
	    float _timeSec)
	{
		Data().currentTimeSec = _timeSec;
	}
};