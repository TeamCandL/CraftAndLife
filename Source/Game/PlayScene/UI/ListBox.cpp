#include "pch\pch.h"
#include "ListBox.h"
#include <Game/System/AbstructInputSystem.h>

void UI::ListBox::Start() {}

void UI::ListBox::Update()
{
	auto& padInput = Get<AbstructInputSystem>().GetPadInputSystem();
	if (padInput.IsFlickedDown(GamePadInputStick::Left))
	{
		MoveFocus(1);
	}
	if (padInput.IsFlickedUp(GamePadInputStick::Left))
	{
		MoveFocus(-1);
	}

	for (int i = 0; i < elements.size(); i++)
	{
		Get<ListElement>(elements[i]).Update((i == currentIndex));
	}
}

std::string UI::ListBox::GetSelectionData()
{
	return Get<ListElement>(elements[currentIndex]).Data().value;
}

bool UI::ListBox::IsPushed()
{
	return Get<AbstructInputSystem>().GetPlayerInput()->IsInteractBegin();
}

void UI::ListBox::PushBackElement(
    const ListElement& _element)
{
	elements.push_back(_element.ToGameObject());
}

void UI::ListBox::SetActiveAll(
    const bool _isActive)
{
	for (auto& obj : elements)
	{
		auto& collection = GetScene().RefGameObjectCollection();
		collection.At<IObject>(obj).SetActive(_isActive);
	}
}

void UI::ListBox::MoveFocus(
    int _dir)
{
	currentIndex += _dir;
	currentIndex = (currentIndex + elements.size()) % elements.size();
}

void UI::ListElement::Update(
    bool _selected)
{
	if (_selected)
	{
		sprite.texture = textureHover;
	}
	else
	{
		sprite.texture = textureNormal;
	}
}
