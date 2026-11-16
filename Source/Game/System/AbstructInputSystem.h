#pragma once
#include <Game/Game.h>
#include "AbstructInputSystem/IPlayerInput.h"
#include "AbstructInputSystem/IUIInput.h"
#include "PadInputSystem.h"
#include <Base/Manager/RawMouseInputManager.h>
#include "InputSystem.h"

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
	InputSystem& GetInputSystem();

	/// @brief 抽象入力プレイヤーを設定する
	/// @tparam T 抽象入力プレイヤーの型
	template <std::derived_from<IPlayerInput> T> void SetPlayerInput();

	/// @brief 抽象入力をシェアポインタから直接設定する
	/// @tparam T 抽象入力プレイヤーの型
	/// @param _pPlayerInput 抽象入力のプレイヤーのシェアポインタ
	template <std::derived_from<IPlayerInput> T> void SetPlayerInput(std::shared_ptr<T> _pPlayerInput);

	/// @brief 抽象入力UIを設定する
	/// @tparam T 抽象入力UIの型
	template <std::derived_from<IUIInput> T> void SetUIInput();

	/// @brief 抽象入力UIをシェアポインタから直接設定する
	/// @tparam T 抽象入力UIの型
	/// @param _pUIInput 抽象入力UIのシェアポインタ
	template <std::derived_from<IUIInput> T> void SetUIInput(std::shared_ptr<T> _pUIInput);

	/// @brief 抽象プレイヤー入力を取得
	/// @return 抽象プレイヤー入力のシェアポインタ
	std::shared_ptr<IPlayerInput> GetPlayerInput() const
	{
		return pPlayerInput;
	}
	/// @brief 抽象UI入力を取得
	/// @return 抽象UI入力のシェアポインタ
	std::shared_ptr<IUIInput> GetUIInput() const
	{
		return pUIInput;
	}

	/// @brief 抽象入力を初期値に戻す
	void ResetInput();
};

#include "AbstructInputSystem.inl"
