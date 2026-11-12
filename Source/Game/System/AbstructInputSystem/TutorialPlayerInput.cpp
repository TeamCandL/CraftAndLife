#include "pch\pch.h"
#include "TutorialPlayerInput.h"

Vec2 TutorialPlayerInput::GetMoveBody() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::GetMoveBody();
	}
}

Vec2 TutorialPlayerInput::GetMoveCamera() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::GetMoveCamera();
	}
}

float TutorialPlayerInput::GetActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::GetActionTrigger();
	}
}

float TutorialPlayerInput::GetSubActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::GetSubActionTrigger();
	}
}

bool TutorialPlayerInput::IsActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsActionBegin();
	}
}

bool TutorialPlayerInput::IsAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsAction();
	}
}

bool TutorialPlayerInput::IsActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsActionEnd();
	}
}

bool TutorialPlayerInput::IsSubActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsSubActionBegin();
	}
}

bool TutorialPlayerInput::IsSubAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsSubAction();
	}
}

bool TutorialPlayerInput::IsSubActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsSubActionEnd();
	}
}

bool TutorialPlayerInput::IsMovedHotbarLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsMovedHotbarLeft();
	}
}

bool TutorialPlayerInput::IsMovedHotbarRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsMovedHotbarRight();
	}
}

bool TutorialPlayerInput::IsInteractBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsInteractBegin();
	}
}

bool TutorialPlayerInput::IsInteract() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsInteract();
	}
}

bool TutorialPlayerInput::IsInteractEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsInteractEnd();
	}
}

bool TutorialPlayerInput::IsJumpBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsJumpBegin();
	}
}

bool TutorialPlayerInput::IsJump() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsJump();
	}
}

bool TutorialPlayerInput::IsJumpEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalPlayerInput::IsJumpEnd();
	}
}
