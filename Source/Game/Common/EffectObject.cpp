#include "EffectObject.h"
EffectObjectData::EffectObjectData(
    const fs::path& _effectFilePath) :
    effect{ _effectFilePath }
{
}

void EffectObject::Start() {}

void EffectObject::Play()
{
	effectPlayer.handle = Get<EffectSystem>().Play(effect);
	effectPlayer.ref    = effect.Get().ref;
}

void EffectObject::OnDestroy()
{
	effectPlayer.destroyMe = true;
}

void EffectObject::SetLoop(
    bool _isLoop)
{
	effectPlayer.isLoop = _isLoop;
}

void EffectObject::SetPlaySpeed(
    float _playSpeed)
{
	effectPlayer.playSpeed = _playSpeed;
}

void EffectObject::SetPosition(
    const Vec3& _position)
{
	effectPlayer.position = _position;
}

void EffectObject::SetRotation(
    const Quaternion& _rotation)
{
	effectPlayer.rotation = _rotation;
}

void EffectObject::SetScale(
    const Vec3& _scale)
{
	effectPlayer.scale = _scale;
}
