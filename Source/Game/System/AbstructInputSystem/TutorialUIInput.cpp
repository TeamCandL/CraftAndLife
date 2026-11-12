#include "pch\pch.h"
#include "TutorialUIInput.h"

bool TutorialUIInput::IsMovedUp() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedUp();
	}
}

bool TutorialUIInput::IsMovedDown() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedDown();
	}
}

bool TutorialUIInput::IsMovedLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedLeft();
	}
}

bool TutorialUIInput::IsMovedRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsMovedRight();
	}
}

bool TutorialUIInput::IsSelected() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsSelected();
	}
}

bool TutorialUIInput::IsChanneled() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return __super::IsChanneled();
	}
}
