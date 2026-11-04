#pragma once
#include <Game/Game.h>

struct EffectObjectData
{
	EffectObjectData(const fs::path& _effectFilePath);
	EffectObjectData(const fs::path& _effectFilePath, const Vec3& _position);
	EffectObjectData(const fs::path& _effectFilePath, const GameObject& _parent);

	Resource<Effect> effect{};
	Component<Transform> transform{};
	Component<EffectPlayer> effectPlayer{};
};

struct EffectObject : ObjectBase<EffectObjectData>
{
	using ObjectBase<EffectObjectData>::ObjectBase;
	void Update() override;
	void Start() override;
	void Play();
	void OnDestroy() override;
};

struct OnceEffectObject : EffectObject
{
	using EffectObject::EffectObject;
	void Start() override;
	void Update() override;
};