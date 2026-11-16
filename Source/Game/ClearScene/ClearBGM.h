#pragma once
#include <Game/Game.h>
#include "ResultType.h"

struct ClearBGMData
{
	Resource<Audio> bgm;
	float volume{ 1.0f };

	ClearBGMData(
	    ResultType _resultType)
	{
		if (_resultType == ResultType::Clear)
		{
			bgm    = Resource<Audio>{ "BGM_SE/Clear.mp3" };
			volume = 0.5f;  // 音量50％
		}
		else
		{
			bgm    = Resource<Audio>{ "BGM_SE/GAMEOVER.mp3" };
			volume = 0.5f;  // 音量50％
		}
	}
};

struct ClearBGM : ObjectBase<ClearBGMData>
{
	using ObjectBase<ClearBGMData>::ObjectBase;

	void Start() override;
};