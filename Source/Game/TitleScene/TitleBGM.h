#pragma once
#include <Game/Game.h>

struct TitleBGMData
{
	Resource<Audio> titleBGM{ "BGM_SE/Title.mp3" };
};

struct TitleBGM : ObjectBase<TitleBGMData>
{
	using ObjectBase<TitleBGMData>::ObjectBase;

	void Start() override;
};