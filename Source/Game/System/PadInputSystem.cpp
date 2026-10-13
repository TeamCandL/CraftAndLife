#include "pch\pch.h"
#include "PadInputSystem.h"


#pragma comment(lib, "GameInput.lib")

void PadInputSystem::Start()
{
	const HRESULT hResult = GameInputCreate(&pGameInput);
	isConnected = false;

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

	const HRESULT hResult = pGameInput->GetCurrentReading(
		GameInputKindGamepad,
		nullptr,
		&reading);

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
		
		current.leftTrigger = state.leftTrigger;
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

bool PadInputSystem::IsButton(const GameInputGamepadButtons button) const
{
	// bit演算して0か0以外かを判定しているだけ
	return (current.buttons & static_cast<uint32_t>(button)) != 0;
}

bool PadInputSystem::IsButtonTriggered(const GameInputGamepadButtons button) const
{
	const uint32_t MASK{ static_cast<uint32_t>(button) };

	// 今回が押された かつ 前回が押されていなかった
	return (current.buttons & MASK) != 0 && (previous.buttons & MASK) == 0;
}

bool PadInputSystem::IsButtonReleased(const GameInputGamepadButtons button) const
{
	const uint32_t MASK{ static_cast<uint32_t>(button) };

	// 今回が押されていない かつ 前回が押されていた
	return (current.buttons & MASK) == 0 && (previous.buttons & MASK) != 0;
}

Vec2 PadInputSystem::GetLeftStickXY() const
{
	return Vec2{ current.leftStickX, current.leftStickY };
}

Vec2 PadInputSystem::GetRightStickXY() const
{
	return Vec2{ current.rightStickX, current.rightStickY };
}

float PadInputSystem::GetLeftTrigger() const
{
	return current.leftTrigger;
}

float PadInputSystem::GetRightTrigger() const
{
	return current.rightTrigger;
}
