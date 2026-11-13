#include "pch\pch.h"
#include "NormalUIInput.h"

NormalUIInput::NormalUIInput(
    AbstructInputSystem& _abstructInputSystem) :
    abstructInputSystem_{ _abstructInputSystem }
{
}

bool NormalUIInput::IsMovedUp() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedUp(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadUp) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::W) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Up);
}

bool NormalUIInput::IsMovedDown() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedDown(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::S) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadDown) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Down);
}

bool NormalUIInput::IsMovedLeft() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedLeft(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::A) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadLeft) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Left);
}

bool NormalUIInput::IsMovedRight() const
{
	return abstructInputSystem_.GetPadInputSystem().IsFlickedRight(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::D) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadRight) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Right);
}

bool NormalUIInput::IsSelected() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadB) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Enter) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Z);
}

bool NormalUIInput::IsChanneled() const
{
	return abstructInputSystem_.GetPadInputSystem().IsButtonReleased(GameInputGamepadA) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Escape) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::X);
}
