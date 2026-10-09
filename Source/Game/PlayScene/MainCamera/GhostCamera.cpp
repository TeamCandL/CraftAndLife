#include "pch\pch.h"
#include "GhostCamera.h"
#include "../MainCamera.h"

void GhostCamera::Update(MainCameraData& _data)
{
	Vec2& angles = _data.angles;
	Vec3& position = _data.position;
	float& moveSpeedPerSec = _data.moveSpeedPerSec;
	float& rotateSpeedPerSec = _data.rotateSpeedPerSec;
	bool& isHideCursor = _data.isHideCursor;
	
	const float deltaTime = Get<DeltaTime>();
	CursorSystem& cursor = Get<CursorSystem>();
	CursorSystemData& cursorData = cursor.Data();
	InputSystem& input = Get<InputSystem>();
	RawMouseInput& rawMouseInput = Get<cha::RawMouseInput>();
	CameraSystemData& camera = Get<CameraSystem>().Data();

	float rotateSpeed{ rotateSpeedPerSec * deltaTime };

	angles.x += cursorData.moveY * rotateSpeed;
	angles.y += cursorData.moveX * rotateSpeed;

	// 角度に制限をつける
	angles.x = std::clamp(
		angles.x,
		-DirectX::XM_PIDIV2 + 0.01f,
		DirectX::XM_PIDIV2 - 0.01f);

	camera.transform.rotation = DirectX::XMQuaternionRotationRollPitchYaw(
		angles.x,
		angles.y,
		0.0f);

	Vec3 move{};

	if (input.GetKey(KeyCode::W))
		move += camera.transform.Forward();
	if (input.GetKey(KeyCode::A))
		move += camera.transform.Left();
	if (input.GetKey(KeyCode::S))
		move += camera.transform.Back();
	if (input.GetKey(KeyCode::D))
		move += camera.transform.Right();
	if (input.GetKey(KeyCode::Q))
		move += camera.transform.Down();
	if (input.GetKey(KeyCode::E))
		move += camera.transform.Up();

	if (!input.GetKey(KeyCode::LeftControl))
		move *= 1000.0f;

	move *= moveSpeedPerSec * deltaTime;

	position += move;

	camera.transform.position = position;
}
