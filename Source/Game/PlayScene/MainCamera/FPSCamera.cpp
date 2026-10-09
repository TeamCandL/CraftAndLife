#include "pch\pch.h"
#include "FPSCamera.h"
#include "../MainCamera.h"

void FPSCamera::Update(MainCameraData& _data)
{
	Vec2& angles = _data.angles;
	float& rotateSpeedPerSec = _data.rotateSpeedPerSec;

	const float deltaTime = Get<DeltaTime>();
	CursorSystem& cursor = Get<CursorSystem>();
	CursorSystemData& cursorData = cursor.Data();


	float rotateSpeed{ rotateSpeedPerSec * deltaTime };

	angles.x += cursorData.moveY * rotateSpeed;
	angles.y += cursorData.moveX * rotateSpeed;

	// 角度に制限をつける
	angles.x = std::clamp(
		angles.x,
		-DirectX::XM_PIDIV2 + 0.01f,
		DirectX::XM_PIDIV2 - 0.01f);
}
