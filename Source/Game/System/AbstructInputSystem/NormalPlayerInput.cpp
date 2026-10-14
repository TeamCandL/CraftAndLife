#include "pch\pch.h"
#include "NormalPlayerInput.h"

NormalPlayerInput::NormalPlayerInput(AbstructInputSystem& _abstructInputSystem) :
	abstructInputSystem_{ _abstructInputSystem }
{
}

Vec2 NormalPlayerInput::GetMoveBody() const
{
	return abstructInputSystem_.GetPadInputSystem().GetLeftStickXY();
}

Vec2 NormalPlayerInput::GetMoveCamera() const
{
	return abstructInputSystem_.GetPadInputSystem().GetRightStickXY();
}

bool NormalPlayerInput::IsActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Right);
}

bool NormalPlayerInput::IsAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Right);
}

bool NormalPlayerInput::IsActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Right);
}

bool NormalPlayerInput::IsSubActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Left);
}

bool NormalPlayerInput::IsSubAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Left);
}

bool NormalPlayerInput::IsSubActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Left);
}

bool NormalPlayerInput::IsMovedHotbarLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadLeftShoulder);
}

bool NormalPlayerInput::IsMovedHotbarRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadRightShoulder);
}

bool NormalPlayerInput::IsInteractBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadB);
}

bool NormalPlayerInput::IsInteract() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadB);
}

bool NormalPlayerInput::IsInteractEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB);
}

bool NormalPlayerInput::IsJumpBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadA);
}

bool NormalPlayerInput::IsJump() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadA);
}

bool NormalPlayerInput::IsJumpEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA);
}
