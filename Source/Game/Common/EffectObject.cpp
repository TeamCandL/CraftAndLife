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
