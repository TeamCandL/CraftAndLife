#pragma once
#include "NormalUIInput.h"
#include "../AbstructInputSystem.h"
#include "../../PlayScene/TutorialSystem.h"

class TutorialUIInput : public NormalUIInput
{
  public:
	TutorialUIInput(
	    TutorialSystem& _tutorialSystem,
	    AbstructInputSystem& _abstructInputSystem) :
	    tutorialSystem_{ _tutorialSystem },
	    NormalUIInput{ _abstructInputSystem }
	{
	}

	// IUIInput を介して継承されました
	bool IsMovedUp() override;
	bool IsMovedDown() override;
	bool IsMovedLeft() override;
	bool IsMovedRight() override;
	bool IsSelected() const override;
	bool IsChanneled() const override;

  private:
	TutorialSystem& tutorialSystem_;
};
