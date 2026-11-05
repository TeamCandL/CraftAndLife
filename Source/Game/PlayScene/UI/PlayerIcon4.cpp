#include "pch\pch.h"
#include "PlayerIcon4.h"

void PlayerIcon4::Start() {}

void PlayerIcon4::Update() {}

void PlayerIcon4::SetPosition(
    const float _angle,
    const float _radius)
{
	transform.position.x = std::cosf(_angle) * _radius + miniMapCenterPosition.x - (transform.scale.x * 0.5f);

	transform.position.y = std::sinf(_angle) * _radius + miniMapCenterPosition.y - (transform.scale.y * 0.5f);
}