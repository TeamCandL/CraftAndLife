#pragma once
#include <Game/Game.h>

struct ClearBGMData
{
	Resource<Audio> clearBGM{ "BGM_SE/Clear.mp3" };
};

struct ClearBGM : ObjectBase<ClearBGMData>
{
	void Start() override;
};