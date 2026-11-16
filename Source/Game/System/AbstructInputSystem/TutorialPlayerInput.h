#pragma once
#include "IPlayerInput.h"
#include "NormalPlayerInput.h"
#include "../AbstructInputSystem.h"
#include "../../PlayScene/TutorialSystem.h"

/// @brief チュートリアル中のプレイヤー入力
class TutorialPlayerInput : public NormalPlayerInput
{
  public:
	TutorialPlayerInput(
	    TutorialSystem& _tutorialSystem,
	    AbstructInputSystem& _abstructInputSystem) :
	    tutorialSystem_{ _tutorialSystem },
	    NormalPlayerInput{ _abstructInputSystem }
	{
	}

	//// IPlayerInput を介して継承されました
	Vec2 GetMoveBody() const override;
	Vec2 GetMoveCamera() const override;
	float GetActionTrigger() const override;
	float GetSubActionTrigger() const override;
	bool IsActionBegin() const override;
	bool IsAction() const override;
	bool IsActionEnd() const override;
	bool IsSubActionBegin() const override;
	bool IsSubAction() const override;
	bool IsSubActionEnd() const override;
	bool IsMovedHotbarLeft() const override;
	bool IsMovedHotbarRight() const override;
	bool IsInteractBegin() const override;
	bool IsInteract() const override;
	bool IsInteractEnd() const override;
	bool IsJumpBegin() const override;
	bool IsJump() const override;
	bool IsJumpEnd() const override;

  private:
	TutorialSystem& tutorialSystem_;
};
