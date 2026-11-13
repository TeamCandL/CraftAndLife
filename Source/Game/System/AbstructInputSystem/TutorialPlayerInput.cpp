#include "pch\pch.h"
#include "TutorialPlayerInput.h"

Vec2 TutorialPlayerInput::GetMoveBody() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_MoveBody:
	case TutorialForcusType_JumpBody:
	case TutorialForcusType_None_Max:
		return __super::GetMoveBody();
	default:
		return Vec2::Zero();
	}
}

Vec2 TutorialPlayerInput::GetMoveCamera() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_MoveCamera:
	case TutorialForcusType_MoveBody:
	case TutorialForcusType_JumpBody:
	case TutorialForcusType_None_Max:
		return __super::GetMoveCamera();
	default:
		return Vec2::Zero();
	}
}

float TutorialPlayerInput::GetActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::GetActionTrigger();
	default:
		return 0.0f;
	}
}

float TutorialPlayerInput::GetSubActionTrigger() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::GetSubActionTrigger();
	default:
		return 0.0f;
	}
}

bool TutorialPlayerInput::IsActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_OpenBackpack:
	case TutorialForcusType_CloseBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsActionBegin();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_OpenBackpack:
	case TutorialForcusType_CloseBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsAction();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_OpenBackpack:
	case TutorialForcusType_CloseBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsActionEnd();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsSubActionBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsSubActionBegin();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsSubAction() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsSubAction();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsSubActionEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsSubActionEnd();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsMovedHotbarLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_SelectBackpack:
	case TutorialForcusType_OpenBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsMovedHotbarLeft();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsMovedHotbarRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_SelectBackpack:
	case TutorialForcusType_OpenBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsMovedHotbarRight();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsInteractBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsInteractBegin();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsInteract() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsInteract();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsInteractEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsInteractEnd();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsJumpBegin() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_JumpBody:
	case TutorialForcusType_None_Max:
		return __super::IsJumpBegin();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsJump() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_JumpBody:
	case TutorialForcusType_None_Max:
		return __super::IsJump();
	default:
		return false;
	}
}

bool TutorialPlayerInput::IsJumpEnd() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_JumpBody:
	case TutorialForcusType_None_Max:
		return __super::IsJumpEnd();
	default:
		return false;
	}
}
