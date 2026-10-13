#pragma once
#include <Game/Game.h>
#include <GameInput.h>

/// @brief ゲームパッド入力システムのデータ
struct PadInputSystemData
{
	/// @brief ゲームパッド入力状態
	struct PadState
	{
		uint32_t buttons;  ///< 各ボタンの on/off ビット演算必須

		float leftStickX;  ///< 左スティックのX軸
		float leftStickY;  ///< 左スティックのY軸

		float rightStickX;  ///< 右スティックのX軸
		float rightStickY;  ///< 右スティックのY軸

		float leftTrigger;   ///< 左トリガー
		float rightTrigger;  ///< 右トリガー
	};

	PadState current{};   ///< 今の入力状態
	PadState previous{};  ///< 前の入力状態

	IGameInput* pGameInput{ nullptr };  ///< ゲームインプット本体
	bool isConnected{ false };          ///< ゲームパッドが接続されているか
};

/// @brief ゲームパッド入力システム
struct PadInputSystem : SystemObjectBase<PadInputSystem, PadInputSystemData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 削除処理
	void OnDestroy() override;

	/// @brief コントローラーが繋がっているか
	/// @return つながっている true / false
	bool IsConnected() const;

	/// @brief 指定のボタンが押されているか
	/// @param button 指定のボタン
	/// @return 押されている true / false
	bool IsButton(const GameInputGamepadButtons button) const;
	/// @brief 指定のボタンが押された瞬間か
	/// @param button 指定のボタン
	/// @return 押された瞬間 true / false
	bool IsButtonTriggered(const GameInputGamepadButtons button) const;
	/// @brief 指定のボタンが離された瞬間か
	/// @param button 指定のボタン
	/// @return 離された瞬間 true / false
	bool IsButtonReleased(const GameInputGamepadButtons button) const;

	/// @brief 左スティックの入力を取得
	/// @return 入力Vec2
	Vec2 GetLeftStickXY() const;
	/// @brief 右スティックの入力を取得
	/// @return 入力Vec2
	Vec2 GetRightStickXY() const;
	/// @brief 左トリガーの入力を取得
	/// @return 入力float
	float GetLeftTrigger() const;
	/// @brief 右トリガーの入力を取得
	/// @return 入力float
	float GetRightTrigger() const;
};
