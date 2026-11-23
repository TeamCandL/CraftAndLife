#include "pch\pch.h"
#include "NormalUIInput.h"

NormalUIInput::NormalUIInput(
    AbstructInputSystem& _abstructInputSystem) :
    abstructInputSystem_{ _abstructInputSystem }
{
}

bool NormalUIInput::IsMovedUp()
{
	if (autoInputValues[InputDirectionType_Up])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedUp(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadUp) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::W) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Up);
}

bool NormalUIInput::IsMovedDown()
{
	if (autoInputValues[InputDirectionType_Down])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedDown(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::S) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadDown) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Down);
}

bool NormalUIInput::IsMovedLeft()
{
	if (autoInputValues[InputDirectionType_Left])
	{
		return true;
	}

	return abstructInputSystem_.GetPadInputSystem().IsFlickedLeft(GamePadInputStick::Left) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::A) ||
	       abstructInputSystem_.GetPadInputSystem().IsButtonTriggered(
	           GameInputGamepadButtons::GameInputGamepadDPadLeft) ||
	       abstructInputSystem_.GetInputSystem().GetKeyDown(KeyCode::Left);
}

bool NormalUIInput::IsMovedRight()
{
	if (autoInputValues[InputDirectionType_Right])
	{
		return true;
	}

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

void NormalUIInput::Update()
{
	// 全方向のパルス入力を更新
	for (InputDirectionType type{}; type < InputDirectionType_Max; type++)
	{
		autoInputValues[type] =
		    UpdateAutoInput(type, abstructInputSystem_.GetPadInputSystem().IsOn(GamePadInputStick::Left, type));
	}
}

bool NormalUIInput::UpdateAutoInput(
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
