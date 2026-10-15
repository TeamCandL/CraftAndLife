#pragma once
#include <Game/Game.h>
#include "AbstructInputSystem/IPlayerInput.h"
#include "AbstructInputSystem/IUIInput.h"
#include "PadInputSystem.h"
#include <Base/Manager/RawMouseInputManager.h>

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
	RawMouseInputManager& GetRawMouseInputManager();

	template<typename T>
	void SetPlayerInput();

	template<typename T>
	void SetUIInput();


	/// @brief 抽象プレイヤー入力を取得
	/// @return 抽象プレイヤー入力のシェアポインタ
	std::shared_ptr<IPlayerInput> GetPlayerInput() { return pPlayerInput; }
	/// @brief 抽象UI入力を取得
	/// @return 抽象UI入力のシェアポインタ
	std::shared_ptr<IUIInput> GetUIInput() { return pUIInput; }
};

#include "AbstructInputSystem.inl"
