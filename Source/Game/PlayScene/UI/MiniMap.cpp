#include "pch\pch.h"
#include "MiniMap.h"
#include "MiniMapPoint.h"

void MiniMap::Start()
{
	Vec2 centerPosition{ 1746, 174 };
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_east.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_south.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_west.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_north.png", centerPosition));

	for (float angle = 0; angle < DirectX::XM_2PI; angle += DirectX::XM_PIDIV2)
	{
		Get<MiniMapPoint>(points[static_cast<int>(angle / DirectX::XM_PIDIV2)]).SetPosition(angle, 150);
	}
}

void MiniMap::Update()
{
	for (float angle = 0; angle < DirectX::XM_2PI; angle += DirectX::XM_PIDIV2)
	{
		Get<MiniMapPoint>(points[static_cast<int>(angle / DirectX::XM_PIDIV2)])
		    .SetPosition(angle + DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4, 150);
	}
}
