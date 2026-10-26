#include "MiniMapPoint.h"

void MiniMapPoint::SetPosition(
    const float _angle,
    const float _radius)
{
	// 極座標から変換
	transform.position.x = std::cosf(_angle) * _radius + miniMapCenterPosition.x - (transform.scale.x * 0.5f);
	transform.position.y = std::sinf(_angle) * _radius + miniMapCenterPosition.y - (transform.scale.y * 0.5f);
}
