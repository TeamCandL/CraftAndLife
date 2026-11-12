#pragma once
#include "IPlayerInput.h"
#include "NormalPlayerInput.h"
#include "../AbstructInputSystem.h"

/// @brief チュートリアル中のプレイヤー入力
class TutorialPlayerInput : public NormalPlayerInput
{
  public:
	using NormalPlayerInput::NormalPlayerInput;

	///// @brief コンストラクタ
	///// @param _abstructInputSystem 抽象入力システムの参照ポインタ
	// TutorialPlayerInput(AbstructInputSystem& _abstructInputSystem);
	///// @brief デストラクタ
	//~TutorialPlayerInput() = default;

	//// IPlayerInput を介して継承されました
	// Vec2 GetMoveBody() const override;
	// Vec2 GetMoveCamera() const override;
	// float GetActionTrigger() const override;
	// float GetSubActionTrigger() const override;
	// bool IsActionBegin() const override;
	// bool IsAction() const override;
	// bool IsActionEnd() const override;
	// bool IsSubActionBegin() const override;
	// bool IsSubAction() const override;
	// bool IsSubActionEnd() const override;
	// bool IsMovedHotbarLeft() const override;
	// bool IsMovedHotbarRight() const override;
	// bool IsInteractBegin() const override;
	// bool IsInteract() const override;
	// bool IsInteractEnd() const override;
	// bool IsJumpBegin() const override;
	// bool IsJump() const override;
	// bool IsJumpEnd() const override;
};
