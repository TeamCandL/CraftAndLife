#pragma once
#include <Game/Game.h>

struct EffectObjectData
{
	EffectObjectData(const fs::path& _effectFilePath);
	Resource<Effect> effect{};
	Component<Transform> transform{};
	Component<EffectPlayer> effectPlayer{};
};

struct EffectObject : ObjectBase<EffectObjectData>
{
	using ObjectBase<EffectObjectData>::ObjectBase;
	void Start() override;
	void Play();
	void OnDestroy() override;
};