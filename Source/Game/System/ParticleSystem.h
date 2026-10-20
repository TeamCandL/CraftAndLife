#pragma once
#include <Game/Game.h>

struct ParticleParameter
{
};

struct ParticleSystemData
{
};

struct ParticleSystem : SystemObjectBase<ParticleSystem, ParticleSystemData>
{
	void Start() override;
	void Update() override;
};