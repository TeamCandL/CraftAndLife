#pragma once
#include <Game/Game.h>
#include "ResultType.h"

struct ClearSceneInputData
{
	ClearSceneInputData(ResultType _resultType);
	float elapsedTime{ 0.0f };
	float inputDelay{ 4.0f };
	GameObject objInputImage{};
	Resource<ResourceType::Texture> texture{};
	ResultType resultType{};
};

struct ClearSceneInput : ObjectBase<ClearSceneInputData>
{
	using ObjectBase<ClearSceneInputData>::ObjectBase;
	void Start() override;
	void Update() override;
};