#include "pch\pch.h"
#include "PlayerIcon2.h"

void PlayerIcon2::Start() {}

void PlayerIcon2::Update() {}

// ミニマップ上のアイコンの位置を設定する
void PlayerIcon2::SetPosition(
    const float _angle,
    const float _radius)
{
	transform.position.x = std::cosf(_angle) * _radius + miniMapCenterPosition.x - (transform.scale.x * 0.5f);

	transform.position.y = std::sinf(_angle) * _radius + miniMapCenterPosition.y - (transform.scale.y * 0.5f);
}