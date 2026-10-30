#include "pch\pch.h"
#include "PlayerIcon3.h"

void PlayerIcon3::Start() {}

void PlayerIcon3::Update() {}

void PlayerIcon3::SetPosition(
    const float _angle,
    const float _radius)
{
	transform.position.x = std::cosf(_angle) * _radius + miniMapCenterPosition.x - (transform.scale.x * 0.5f);

	transform.position.y = std::sinf(_angle) * _radius + miniMapCenterPosition.y - (transform.scale.y * 0.5f);
}