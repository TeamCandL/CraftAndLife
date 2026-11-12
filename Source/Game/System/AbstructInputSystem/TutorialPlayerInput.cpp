#include "pch\pch.h"
#include "TutorialPlayerInput.h"

Vec2 TutorialPlayerInput::GetMoveBody() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::GetMoveBody();
	}
}

Vec2 TutorialPlayerInput::GetMoveCamera() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::GetMoveCamera();
	}
}

float TutorialPlayerInput::GetActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::GetActionTrigger();
	}
}

float TutorialPlayerInput::GetSubActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::GetSubActionTrigger();
	}
}

bool TutorialPlayerInput::IsActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsActionBegin();
	}
}

bool TutorialPlayerInput::IsAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsAction();
	}
}

bool TutorialPlayerInput::IsActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsActionEnd();
	}
}

bool TutorialPlayerInput::IsSubActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsSubActionBegin();
	}
}

bool TutorialPlayerInput::IsSubAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsSubAction();
	}
}

bool TutorialPlayerInput::IsSubActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsSubActionEnd();
	}
}

bool TutorialPlayerInput::IsMovedHotbarLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedHotbarLeft();
	}
}

bool TutorialPlayerInput::IsMovedHotbarRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedHotbarRight();
	}
}

bool TutorialPlayerInput::IsInteractBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsInteractBegin();
	}
}

bool TutorialPlayerInput::IsInteract() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsInteract();
	}
}

bool TutorialPlayerInput::IsInteractEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsInteractEnd();
	}
}

bool TutorialPlayerInput::IsJumpBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsJumpBegin();
	}
}

bool TutorialPlayerInput::IsJump() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsJump();
	}
}

bool TutorialPlayerInput::IsJumpEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsJumpEnd();
	}
}
