#include "pch\pch.h"
#include "PadInputSystem.h"

#pragma comment(lib, "GameInput.lib")

void PadInputSystem::Start()
{
	const HRESULT hResult = GameInputCreate(&pGameInput);
	isConnected           = false;

	if (FAILED(hResult))
	{
		pGameInput = nullptr;  // 失敗したならnullptr入れとく
	}
}

void PadInputSystem::Update()
{
	// 前回のフレーム状態を保存
	previous = current;

	current = {};

	if (!pGameInput)
	{
		isConnected = false;
		return;  // ゲームインプット本体取得できていなければ未接続
	}

	// 読み取り専用として取得
	IGameInputReading* reading{ nullptr };

	const HRESULT hResult = pGameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &reading);

	if (FAILED(hResult) || !reading)
	{
		isConnected = false;
		return;  // 読み取り失敗
	}

	GameInputGamepadState state{};

	if (reading->GetGamepadState(&state))
	{
		isConnected = true;

		current.buttons = static_cast<uint32_t>(state.buttons);

		current.leftStickX = state.leftThumbstickX;
		current.leftStickY = state.leftThumbstickY;

		current.rightStickX = state.rightThumbstickX;
		current.rightStickY = state.rightThumbstickY;

		current.leftTrigger  = state.leftTrigger;
		current.rightTrigger = state.rightTrigger;
	}

	reading->Release();
}

void PadInputSystem::OnDestroy()
{
	// 解放処理も忘れずに
	if (pGameInput)
	{
		pGameInput->Release();
		pGameInput = nullptr;
	}

	isConnected = false;
}

bool PadInputSystem::IsConnected() const
{
	return isConnected;
}

bool PadInputSystem::IsButton(
    const GamePadInputTriggerButton _button) const
{
	const float CURRENT_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? current.leftTrigger : current.rightTrigger;
	const float PREVIOUS_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? previous.leftTrigger : previous.rightTrigger;
	return CURRENT_TRIGGER > deadZoneTrigger;
}

bool PadInputSystem::IsButton(
    const GameInputGamepadButtons _button) const
{
	// bit演算して0か0以外かを判定しているだけ
	return (current.buttons & static_cast<uint32_t>(_button)) != 0;
}

bool PadInputSystem::IsButtonTriggered(
    const GamePadInputTriggerButton _button) const
{
	const float CURRENT_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? current.leftTrigger : current.rightTrigger;
	const float PREVIOUS_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? previous.leftTrigger : previous.rightTrigger;

	// 今回が押された かつ 前回が押されていなかった
	return CURRENT_TRIGGER > deadZoneTrigger && PREVIOUS_TRIGGER <= deadZoneTrigger;
}

bool PadInputSystem::IsButtonTriggered(
    const GameInputGamepadButtons _button) const
{
	const uint32_t MASK{ static_cast<uint32_t>(_button) };

	// 今回が押された かつ 前回が押されていなかった
	return (current.buttons & MASK) != 0 && (previous.buttons & MASK) == 0;
}

bool PadInputSystem::IsButtonReleased(
    const GameInputGamepadButtons _button) const
{
	const uint32_t MASK{ static_cast<uint32_t>(_button) };

	// 今回が押されていない かつ 前回が押されていた
	return (current.buttons & MASK) == 0 && (previous.buttons & MASK) != 0;
}

bool PadInputSystem::IsButtonReleased(
    const GamePadInputTriggerButton _button) const
{
	const float CURRENT_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? current.leftTrigger : current.rightTrigger;
	const float PREVIOUS_TRIGGER =
	    _button == GamePadInputTriggerButton::Left ? previous.leftTrigger : previous.rightTrigger;

	// 今回が押された かつ 前回が押されていなかった
	return CURRENT_TRIGGER <= deadZoneTrigger && PREVIOUS_TRIGGER > deadZoneTrigger;
}

bool PadInputSystem::IsFlickedRight(
    const GamePadInputStick _stick)
{
	const float CURRENT_X  = _stick == GamePadInputStick::Left ? current.leftStickX : current.rightStickX;
	const float PREVIOUS_X = _stick == GamePadInputStick::Left ? previous.leftStickX : previous.rightStickX;

	return CURRENT_X > deadZoneStick && PREVIOUS_X <= deadZoneStick;
}

bool PadInputSystem::IsFlickedLeft(
    const GamePadInputStick _stick)
{
	const float CURRENT_X  = _stick == GamePadInputStick::Left ? current.leftStickX : current.rightStickX;
	const float PREVIOUS_X = _stick == GamePadInputStick::Left ? previous.leftStickX : previous.rightStickX;

	return CURRENT_X < -deadZoneStick && PREVIOUS_X >= -deadZoneStick;
}

bool PadInputSystem::IsFlickedUp(
    const GamePadInputStick _stick)
{
	const float CURRENT_Y  = _stick == GamePadInputStick::Left ? current.leftStickY : current.rightStickY;
	const float PREVIOUS_Y = _stick == GamePadInputStick::Left ? previous.leftStickY : previous.rightStickY;

	return CURRENT_Y > deadZoneStick && PREVIOUS_Y <= deadZoneStick;
}

bool PadInputSystem::IsFlickedDown(
    const GamePadInputStick _stick)
{
	const float CURRENT_Y  = _stick == GamePadInputStick::Left ? current.leftStickY : current.rightStickY;
	const float PREVIOUS_Y = _stick == GamePadInputStick::Left ? previous.leftStickY : previous.rightStickY;

	return CURRENT_Y < -deadZoneStick && PREVIOUS_Y >= -deadZoneStick;
}

bool PadInputSystem::IsOnRight(
    const GamePadInputStick _stick)
{
	const float CURRENT_X = _stick == GamePadInputStick::Left ? current.leftStickX : current.rightStickX;
	return CURRENT_X > deadZoneStick;
}

bool PadInputSystem::IsOnLeft(
    const GamePadInputStick _stick)
{
	const float CURRENT_X = _stick == GamePadInputStick::Left ? current.leftStickX : current.rightStickX;
	return CURRENT_X < deadZoneStick;
}

bool PadInputSystem::IsOnUp(
    const GamePadInputStick _stick)
{
	const float CURRENT_Y = _stick == GamePadInputStick::Left ? current.leftStickY : current.rightStickY;
	return CURRENT_Y > deadZoneStick;
}

bool PadInputSystem::IsOnDown(
    const GamePadInputStick _stick)
{
	const float CURRENT_Y = _stick == GamePadInputStick::Left ? current.leftStickY : current.rightStickY;
	return CURRENT_Y < deadZoneStick;
}

Vec2 PadInputSystem::GetLeftStickXY() const
{
	return ToCutDeadStick(Vec2{ current.leftStickX, current.leftStickY });
}

Vec2 PadInputSystem::GetRightStickXY() const
{
	return ToCutDeadStick(Vec2{ current.rightStickX, current.rightStickY });
}

float PadInputSystem::GetLeftTrigger() const
{
	return ToCutDeadTrigger(current.leftTrigger);
}

float PadInputSystem::GetRightTrigger() const
{
	return ToCutDeadTrigger(current.rightTrigger);
}

Vec2 PadInputSystem::ToCutDeadStick(
    const Vec2 _input) const
{
	return Vec2{
		_input.x < -deadZoneStick || deadZoneStick < _input.x ? _input.x : 0.0f,
		_input.y < -deadZoneStick || deadZoneStick < _input.y ? _input.y : 0.0f,
	};
}

float PadInputSystem::ToCutDeadTrigger(
    const float _input) const
{
	return _input > deadZoneTrigger ? _input : 0.0f;
}
