#include "EffectObject.h"
EffectObjectData::EffectObjectData(
    const fs::path& _effectFilePath) :
    effect{ _effectFilePath }
{
}

EffectObjectData::EffectObjectData(
    const fs::path& _effectFilePath,
    const Vec3& _position) :
    EffectObjectData(_effectFilePath)
{
	transform.position = _position;
}

EffectObjectData::EffectObjectData(
    const fs::path& _effectFilePath,
    const GameObject& _parent) :
    EffectObjectData(_effectFilePath)
{
	transform.parent = _parent;
}

void EffectObject::Update() {}

void EffectObject::Start() {}

void EffectObject::Play()
{
	effectPlayer.ref = Get<EffectSystem>().Play(effect);
}

void EffectObject::OnDestroy()
{
	effectPlayer.destroyMe = true;
}

void OnceEffectObject::Start()
{
	EffectObject::Start();
	Play();
}

void OnceEffectObject::Update()
{
	EffectObject::Update();
	if (effectPlayer.ref == nullptr)
	{
		effectPlayer.destroyMe = true;
		DestroyMe();
	}
}
