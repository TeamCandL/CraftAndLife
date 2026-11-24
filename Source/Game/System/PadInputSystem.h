#pragma once
#include <Game/Game.h>
#include <GameInput.h>
#include "InputSystem/InputDirectionType.h"

/// @brief ゲームパッド入力のボタンとしてのトリガー部分
enum struct GamePadInputTriggerButton
{
	Left,   // 左トリガー
	Right,  // 右トリガー
};

/// @brief ゲームパッド入力のスティック
enum struct GamePadInputStick
{
	Left,   // 左スティック
	Right,  // 右スティック
};

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

	float deadZoneTrigger{ 0.1f };  ///< トリガーのデッドゾーン
	float deadZoneStick{ 0.1f };    ///< スティックのデッドゾーン
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
	/// @param _button 指定のボタン
	/// @return 押されている true / false
	bool IsButton(const GamePadInputTriggerButton _button) const;
	/// @brief 指定のトリガーボタンが押されているか
	/// @param _button 指定のボタン
	/// @return 押されている true / false
	bool IsButton(const GameInputGamepadButtons _button) const;
	/// @brief 方向ボタンの入力を取得
	/// @param _button
	/// @return
	bool IsButton(const InputDirectionType _dirType) const;
	/// @brief 指定のボタンが押された瞬間か
	/// @param _button 指定のボタン
	/// @return 押された瞬間 true / false
	bool IsButtonTriggered(const GamePadInputTriggerButton _button) const;
	/// @brief 指定のトリガーボタンが押された瞬間か
	/// @param _button 指定のボタン
	/// @return 押された瞬間 true / false
	bool IsButtonTriggered(const GameInputGamepadButtons _button) const;
	/// @brief 指定のボタンが離された瞬間か
	/// @param _button 指定のボタン
	/// @return 離された瞬間 true / false
	bool IsButtonReleased(const GameInputGamepadButtons _button) const;
	/// @brief 指定のトリガーボタンが離された瞬間か
	/// @param _button 指定のボタン
	/// @return 離された瞬間 true / false
	bool IsButtonReleased(const GamePadInputTriggerButton _button) const;

	/// @brief スティックのフリック入力右があったか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsFlickedRight(const GamePadInputStick _stick);
	/// @brief スティックのフリック入力左
	/// があったか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsFlickedLeft(const GamePadInputStick _stick);
	/// @brief スティックのフリック入力上があったか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsFlickedUp(const GamePadInputStick _stick);
	/// @brief スティックのフリック入力下があったか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsFlickedDown(const GamePadInputStick _stick);

	/// @brief スティックの入力右があるか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsOnRight(const GamePadInputStick _stick);
	/// @brief スティックの入力左があるか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsOnLeft(const GamePadInputStick _stick);
	/// @brief スティックの入力上があるか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsOnUp(const GamePadInputStick _stick);
	/// @brief スティックの入力下があるか
	/// @param _stick スティック
	/// @return 入力があった true / false
	bool IsOnDown(const GamePadInputStick _stick);

	/// @brief スティックの指定方向への入力があるか
	/// @param _stick スティック
	/// @param _directionType 方向
	/// @return 入力がある true / false
	bool IsOn(const GamePadInputStick _stick, const InputDirectionType _directionType);

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

	/// @brief スティック入力がデッドゾーンの場合切り捨てる加工
	/// @param _input スティック入力
	/// @return 加工された入力
	Vec2 ToCutDeadStick(const Vec2 _input) const;

	/// @brief トリガー入力がデッドゾーンの場合切り捨てる加工
	/// @param _input トリガー入力
	/// @return 加工された入力
	float ToCutDeadTrigger(const float _input) const;
};
