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

	// 朝・夜の切り替わりSE
	Resource<Audio> morningSE{ "BGM_SE/The sound of morning arriving.wav" };
	Resource<Audio> nightfallSE{ "BGM_SE/The sound of nightfall.mp3" };

	// 現在の時間帯
	bool isNight{ false };

	// 現在の日付（1日目から開始）
	int32_t currentDay{ 1 };

	bool isDayChanged{ true };  ///< 日付が変更されたフレームか

	float currentTimeSec{};
	GameObject objSky{};

	// 昼の環境音
	Resource<Audio> daytimeAmbientSE{ "BGM_SE/Daytime ambient sounds.mp3" };

	// 夜の環境音
	Resource<Audio> nighttimeAmbientSE{ "BGM_SE/Nighttime ambient sounds.mp3" };

	// 環境音の初期化が済んでいるか
	bool isAmbientInitialized{ false };
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

	int32_t GetCurrentDay() const
	{
		return Data().currentDay;
	}

	void SetCurrentDay(
	    int32_t _day)
	{
		Data().currentDay = _day;
	}

	/// @brief 現在が夜かどうかを取得
	/// @return true：夜 / false：昼
	bool IsNight() const
	{
		return Data().isNight;
	}

	/// @brief	現在のフレームで日付が変更されたかどうかを取得する
	/// @return
	bool IsDayChanged() const
	{
		return isDayChanged;
	}
};