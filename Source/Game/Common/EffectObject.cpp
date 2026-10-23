#include "EffectObject.h"
EffectObjectData::EffectObjectData(
    const fs::path& _effectFilePath) :
    effect{ _effectFilePath }
{
}

void EffectObject::Start() {}

void EffectObject::Play()
{
	effectPlayer.ref = Get<EffectSystem>().Play(effect);
}

void EffectObject::OnDestroy()
{
	effectPlayer.destroyMe = true;
}
