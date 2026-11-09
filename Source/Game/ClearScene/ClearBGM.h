#pragma once
#include <Game/Game.h>
#include "ResultType.h"

struct ClearBGMData
{
	Resource<Audio> bgm;

	ClearBGMData(
	    ResultType _resultType)
	{
		if (_resultType == ResultType::Clear)
		{
			bgm = Resource<Audio>{ "BGM_SE/Clear.mp3" };
		}
		else
		{
			bgm = Resource<Audio>{ "BGM_SE/GAMEOVER.mp3" };
		}
	}
};

struct ClearBGM : ObjectBase<ClearBGMData>
{
	using ObjectBase<ClearBGMData>::ObjectBase;

	void Start() override;
};