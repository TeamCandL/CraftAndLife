#include "pch\pch.h"
#include "NormalUIInput.h"

NormalUIInput::NormalUIInput(AbstructInputSystem& _abstructInputSystem) :
	abstructInputSystem_{ _abstructInputSystem }
{
}

bool NormalUIInput::IsMovedUp() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedUp(GamePadInputStick::Left);
}

bool NormalUIInput::IsMovedDown() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedDown(GamePadInputStick::Left);
}

bool NormalUIInput::IsMovedLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedLeft(GamePadInputStick::Left);
}

bool NormalUIInput::IsMovedRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedRight(GamePadInputStick::Left);
}

bool NormalUIInput::IsSelected() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB);
}

bool NormalUIInput::IsChanneled() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA);
}
