#include "pch\pch.h"
#include "NormalPlayerInput.h"

NormalPlayerInput::NormalPlayerInput(
    AbstructInputSystem& _abstructInputSystem) :
    abstructInputSystem_{ _abstructInputSystem }
{
}

Vec2 NormalPlayerInput::GetMoveBody() const
{
	InputSystem& inputSystem = abstructInputSystem_.GetInputSystem();
	Vec2 wasdMove{
		inputSystem.GetKey(KeyCode::A)          ? -1.0f
		: 0.0f + inputSystem.GetKey(KeyCode::D) ? +1.0f
		                                        : 0.0f,
		inputSystem.GetKey(KeyCode::S)          ? -1.0f
		: 0.0f + inputSystem.GetKey(KeyCode::W) ? +1.0f
		                                        : 0.0f,
	};

	return abstructInputSystem_.GetPadInputSystem().GetLeftStickXY() + wasdMove;
}

Vec2 NormalPlayerInput::GetMoveCamera() const
{
	RawMouseInputManager rawMouseInputManager = abstructInputSystem_.GetRawMouseInputManager();
	Vec2 mouseMove                            = rawMouseInputManager.GetMoveDelta();

	mouseMove *= 0.04f;

	mouseMove.y *= -1.0f;

	return abstructInputSystem_.GetPadInputSystem().GetRightStickXY() + mouseMove;
}

float NormalPlayerInput::GetActionTrigger() const
{
	return abstructInputSystem_.GetPadInputSystem().GetRightTrigger() +
	               abstructInputSystem_.GetRawMouseInputManager().GetMouse(MouseCode_Left)
	           ? 1.0f
	           : 0.0f;
}

float NormalPlayerInput::GetSubActionTrigger() const
{
	return abstructInputSystem_.GetPadInputSystem().GetLeftTrigger() +
	               abstructInputSystem_.GetRawMouseInputManager().GetMouse(MouseCode_Right)
	           ? 1.0f
	           : 0.0f;
}

bool NormalPlayerInput::IsActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Right) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouseDown(MouseCode_Left);
}

bool NormalPlayerInput::IsAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Right) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouse(MouseCode_Left);
}

bool NormalPlayerInput::IsActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Right) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouseUp(MouseCode_Left);
}

bool NormalPlayerInput::IsSubActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Left) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouseDown(MouseCode_Right);
}

bool NormalPlayerInput::IsSubAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Left) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouse(MouseCode_Right);
}

bool NormalPlayerInput::IsSubActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Left) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetMouseUp(MouseCode_Right);
}

bool NormalPlayerInput::IsMovedHotbarLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadLeftShoulder) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetScrollWheel() > 0;
}

bool NormalPlayerInput::IsMovedHotbarRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadRightShoulder) ||
	       abstructInputSystem_.GetRawMouseInputManager().GetScrollWheel() < 0;
}

bool NormalPlayerInput::IsInteractBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadB) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::E);
}

bool NormalPlayerInput::IsInteract() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadB) ||
	       abstructInputSystem_.GetInputSystem().GetKey(KeyCode::E);
}

bool NormalPlayerInput::IsInteractEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB) ||
	       abstructInputSystem_.GetInputSystem().GetKeyUp(KeyCode::E);
}

bool NormalPlayerInput::IsJumpBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadA) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Space);
}

bool NormalPlayerInput::IsJump() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadA) ||
	       abstructInputSystem_.GetInputSystem().GetKey(KeyCode::Space);
}

bool NormalPlayerInput::IsJumpEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA) ||
	       abstructInputSystem_.GetInputSystem().GetKeyUp(KeyCode::Space);
}
