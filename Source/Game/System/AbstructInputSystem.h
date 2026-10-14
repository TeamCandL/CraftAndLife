#pragma once
#include <Game/Game.h>
#include "AbstructInputSystem/IPlayerInput.h"
#include "AbstructInputSystem/IUIInput.h"
#include "PadInputSystem.h"

/// @brief 抽象的な入力を扱うシステムデータ
struct AbstructInputSystemData
{
	std::shared_ptr<IPlayerInput> pPlayerInput{};
	std::shared_ptr<IUIInput> pUIInput{};
};

/// @brief 抽象的な入力を扱うシステム
struct AbstructInputSystem : SystemObjectBase<AbstructInputSystem, AbstructInputSystemData>
{
	void Start() override;
	void Update() override;

	PadInputSystem& GetPadInputSystem();

	template<typename T>
	void SetPlayerInput();

	template<typename T>
	void SetUIInput();

	std::shared_ptr<IPlayerInput> GetPlayerInput();
	std::shared_ptr<IUIInput> GetUIInput();
};

template<typename T>
inline void AbstructInputSystem::SetPlayerInput()
{
	pPlayerInput = std::make_shared<T>(*this);
}

template<typename T>
inline void AbstructInputSystem::SetUIInput()
{
	pUIInput = std::make_shared<T>(*this);
}
