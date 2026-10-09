#pragma once
#include <Game/Game.h>

struct DestroyTestingData
{
	std::vector<GameObject> gameObjects{};
	uint32_t nameCounter{};
};

struct DestroyTesting : ObjectBase<DestroyTestingData>
{
	void Start() override;
	void Update() override;

	void InstantiateObj();
};
