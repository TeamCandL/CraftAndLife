#include "pch\pch.h"
#include "TutorialUIInput.h"

bool TutorialUIInput::IsMovedUp() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedUp();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedDown() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedDown();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedLeft();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedRight();
	default:
		return false;
	}
}

bool TutorialUIInput::IsSelected() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsSelected();
	default:
		return false;
	}
}

bool TutorialUIInput::IsChanneled() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsChanneled();
	default:
		return false;
	}
}
