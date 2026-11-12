#include "pch\pch.h"
#include "TutorialUIInput.h"

bool TutorialUIInput::IsMovedUp() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsMovedUp();
	}
}

bool TutorialUIInput::IsMovedDown() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsMovedDown();
	}
}

bool TutorialUIInput::IsMovedLeft() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsMovedLeft();
	}
}

bool TutorialUIInput::IsMovedRight() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsMovedRight();
	}
}

bool TutorialUIInput::IsSelected() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsSelected();
	}
}

bool TutorialUIInput::IsChanneled() const
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
	default:
		return NormalUIInput::IsChanneled();
	}
}
