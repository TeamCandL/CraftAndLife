#include "pch\pch.h"
#include "PadOnlyUIInput.h"

PadOnlyUIInput::PadOnlyUIInput(
    AbstructInputSystem& _abstructInputSystem) :
    abstructInputSystem_{ _abstructInputSystem }
{
}

bool PadOnlyUIInput::IsMovedUp() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedUp(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadButtons::GameInputGamepadDPadUp);
}

bool PadOnlyUIInput::IsMovedDown() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedDown(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadDown);
}

bool PadOnlyUIInput::IsMovedLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedLeft(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadLeft);
}

bool PadOnlyUIInput::IsMovedRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedRight(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadRight);
}

bool PadOnlyUIInput::IsSelected() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB);
}

bool PadOnlyUIInput::IsChanneled() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA);
}
