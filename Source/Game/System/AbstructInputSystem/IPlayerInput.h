#pragma once
#include <Game/Game.h>

/// @brief UIの入力抽象化インタフェース
class IPlayerInput
{
public:
	/// @brief コンストラクタ
	IPlayerInput() = default;
	/// @brief デストラクタ
	virtual ~IPlayerInput() = default;

	/// @brief 体の移動入力を取得する
	/// @return 移動入力Vec2 0.0f ~ 1.0f
	virtual Vec2 GetMoveBody() const = 0;
	/// @brief カメラの回転入力を取得する
	/// @return 回転入力Vec2 0.0f ~ 1.0f
	virtual Vec2 GetMoveCamera() const = 0;
	/// @brief アクションの押し込み具合
	/// @return 押し込み具合 0.0f ~ 1.0f
	virtual float GetActionTrigger() const = 0;
	/// @brief アクションの押し込み具合
	/// @return 押し込み具合 0.0f ~ 1.0f
	virtual float GetSubActionTrigger() const = 0;
	/// @brief アクション入力が始まったタイミングか取得
	/// @return アクション入力が始まったタイミング true / false
	virtual bool IsActionBegin() const = 0;
	/// @brief アクション中かを取得
	/// @return アアクション中 true / false
	virtual bool IsAction() const = 0;
	/// @brief アクション入力が終わったタイミングか取得
	/// @return アクション入力が終わったタイミング true / false
	virtual bool IsActionEnd() const = 0;
	/// @brief サブアクション入力が始まったタイミングか取得
	/// @return サブアクション入力が始まったタイミング true / false
	virtual bool IsSubActionBegin() const = 0;
	/// @brief サブアクション中かを取得
	/// @return サブアクション中 true / false
	virtual bool IsSubAction() const = 0;
	/// @brief サブアクション入力が終わったタイミングか取得
	/// @return サブアクション入力が終わったタイミング true / false
	virtual bool IsSubActionEnd() const = 0;
	/// @brief ホットバーの左移動があったかを取得
	/// @return ホットバーの左移動があった true / false
	virtual bool IsMovedHotbarLeft() const = 0;
	/// @brief ホットバーの右移動があったかを取得
	/// @return ホットバーの右移動があった true / false
	virtual bool IsMovedHotbarRight() const = 0;
	/// @brief インタラクト入力が開始されたかを取得
	/// @return インタラクト入力が開始された true / false
	virtual bool IsInteractBegin() const = 0;
	/// @brief インタラクト入力があるかを取得
	/// @return インタラクト入力がある true / false
	virtual bool IsInteract() const = 0;
	/// @brief インタラクト入力が終了されたかを取得
	/// @return インタラクト入力が終了された true / false
	virtual bool IsInteractEnd() const = 0;
	/// @brief ジャンプ入力が開始されたかを取得
	/// @return ジャンプ入力が開始された true / false
	virtual bool IsJumpBegin() const = 0;
	/// @brief ジャンプ入力があるかを取得
	/// @return ジャンプ入力がある true / false
	virtual bool IsJump() const = 0;
	/// @brief ジャンプ入力が終了されたかを取得
	/// @return ジャンプ入力が終了された true / false
	virtual bool IsJumpEnd() const = 0;
};
