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
	void SetLoop(bool _isLoop);
	void SetPlaySpeed(float _playSpeed);
	void SetPosition(const Vec3& _position);
	void SetRotation(const Quaternion& _rotation);
	void SetScale(const Vec3& _scale);
};