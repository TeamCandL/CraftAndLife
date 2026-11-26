#include "pch\pch.h"
#include "Animal.h"

void Animal::Start() {}

void Animal::Update() {}

void Animal::Move(
    Vec3 target_,
    Vec3 pos_,
    Vec3 Fwd_,
    float spd_)
{
}

void Animal::Draw() {}

void Animal::DrawShadowMap() {}

void Animal::OnDestroy() {}

void Animal::Damaged(
    int32_t damage)
{
}

void Animal::Damaged(
    int32_t damage,
    bool isGreatsword)
{
}

void Animal::PlayAnim(
    int32_t animIndex_,
    AnimState state_)
{
}

void Animal::SetWanderingCheckPoint(
    Vec3 position_)
{
}

void Animal::DropItem() {}

void Animal::RegisterStates(
    StateRegistry& registry) const
{
}

void Animal::StartDeathAnimation() {}

void Animal::UpdateDeathAnimation() {}
