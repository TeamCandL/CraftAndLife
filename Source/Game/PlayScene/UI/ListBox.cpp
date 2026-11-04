#include "pch\pch.h"
#include "ListBox.h"
#include <Game/System/AbstructInputSystem.h>

void UI::ListBox::Start()
{
	selectedElementCursor = GetScene().Instantiate<SelectedElementCursor>();
	Get<SelectedElementCursor>(selectedElementCursor).Data().transform.layerOrder = 6;
	Get<SelectedElementCursor>(selectedElementCursor).Data().transform.scale      = Vec2{ 32, 32 };
}

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
	UITransform& elementTransform = Get<UITransform>(elements[currentIndex]);
	UIImage& elementImage         = Get<UIImage>(elements[currentIndex]);

	// 選択項目の矢印の位置
	UITransform& cursorTransform = Get<UITransform>(selectedElementCursor);
	cursorTransform.position     = elementTransform.position + Vec2{ 0.0f, 1.0f };
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
	Get<SelectedElementCursor>(selectedElementCursor).SetActive(_isActive);
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
