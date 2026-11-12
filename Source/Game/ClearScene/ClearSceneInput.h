#pragma once
#include <Game/Game.h>

struct ClearSceneInputData
{
	float elapsedTime{ 0.0f };
	float inputDelay{ 4.0f };
};

struct ClearSceneInput : ObjectBase<ClearSceneInputData>
{
	void Update() override;
};