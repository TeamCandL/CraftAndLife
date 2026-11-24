#include "pch\pch.h"
#include "PadOnlyUIInput.h"

PadOnlyUIInput::PadOnlyUIInput(
    AbstructInputSystem& _abstructInputSystem) :
    abstructInputSystem_{ _abstructInputSystem }
{
}

bool PadOnlyUIInput::IsMovedUp()
{
	if (autoInputValues[InputDirectionType_Up])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedUp(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(GameInputGamepadButtons::GameInputGamepadDPadUp);
}

bool PadOnlyUIInput::IsMovedDown()
{
	if (autoInputValues[InputDirectionType_Down])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedDown(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadDown);
}

bool PadOnlyUIInput::IsMovedLeft()
{
	if (autoInputValues[InputDirectionType_Left])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedLeft(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadLeft);
}

bool PadOnlyUIInput::IsMovedRight()
{
	if (autoInputValues[InputDirectionType_Right])
	{
		return true;
	}

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

void PadOnlyUIInput::Update() {}

bool PadOnlyUIInput::UpdateAutoInput(
    const InputDirectionType _directionType,
    const bool _isOn)
{
	if (_isOn)
	{
		autoInputWaitTimeLefts[_directionType] -= ManagerRegistry::Ref<DeltaTime>();
		if (autoInputWaitTimeLefts[_directionType] <= 0.0f)
		{
			autoInputTimeLefts[_directionType] -= ManagerRegistry::Ref<DeltaTime>();
			if (autoInputTimeLefts[_directionType] <= 0.0f)
			{
				autoInputTimeLefts[_directionType] = AUTO_INPUT_DISTANCE_SEC;
				return true;
			}
		}
	}
	else
	{
		autoInputWaitTimeLefts[_directionType] = AUTO_INPUT_WAIT_THRESHOLD_SEC;
		autoInputTimeLefts[_directionType]     = 0.0f;
	}

	return false;
}
