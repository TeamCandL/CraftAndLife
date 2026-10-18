#include "pch\pch.h"
#include "PadOnlyPlayerInput.h"

PadOnlyPlayerInput::PadOnlyPlayerInput(AbstructInputSystem& _abstructInputSystem) :
	abstructInputSystem_{ _abstructInputSystem }
{
}

Vec2 PadOnlyPlayerInput::GetMoveBody() const
{
	return abstructInputSystem_.GetPadInputSystem().GetLeftStickXY();
}

Vec2 PadOnlyPlayerInput::GetMoveCamera() const
{
	return abstructInputSystem_.GetPadInputSystem().GetRightStickXY();
}

float PadOnlyPlayerInput::GetActionTrigger() const
{
	return abstructInputSystem_.GetPadInputSystem().GetRightTrigger();
}

float PadOnlyPlayerInput::GetSubActionTrigger() const
{
	return abstructInputSystem_.GetPadInputSystem().GetLeftTrigger();
}

bool PadOnlyPlayerInput::IsActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Right);
}

bool PadOnlyPlayerInput::IsAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Right);
}

bool PadOnlyPlayerInput::IsActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Right);
}

bool PadOnlyPlayerInput::IsSubActionBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GamePadInputTriggerButton::Left);
}

bool PadOnlyPlayerInput::IsSubAction() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GamePadInputTriggerButton::Left);
}

bool PadOnlyPlayerInput::IsSubActionEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GamePadInputTriggerButton::Left);
}

bool PadOnlyPlayerInput::IsMovedHotbarLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadLeftShoulder);
}

bool PadOnlyPlayerInput::IsMovedHotbarRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadRightShoulder);
}

bool PadOnlyPlayerInput::IsInteractBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadB);
}

bool PadOnlyPlayerInput::IsInteract() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadB);
}

bool PadOnlyPlayerInput::IsInteractEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB);
}

bool PadOnlyPlayerInput::IsJumpBegin() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadA);
}

bool PadOnlyPlayerInput::IsJump() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButton(GameInputGamepadA);
}

bool PadOnlyPlayerInput::IsJumpEnd() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA);
}
