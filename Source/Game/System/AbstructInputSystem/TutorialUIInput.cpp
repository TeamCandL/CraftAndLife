#include "pch\pch.h"
#include "TutorialUIInput.h"

bool TutorialUIInput::IsMovedUp()
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedUp();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedDown()
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedDown();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedLeft()
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_None_Max:
		return __super::IsMovedLeft();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedRight()
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

bool TutorialUIInput::IsMovedLeftTab()
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_SelectBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsMovedLeftTab();
	default:
		return false;
	}
}

bool TutorialUIInput::IsMovedRightTab()
{
	switch (tutorialSystem_.GetFocusType())
	{
	case TutorialForcusType_SelectBackpack:
	case TutorialForcusType_None_Max:
		return __super::IsMovedRightTab();
	default:
		return false;
	}
}
