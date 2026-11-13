#include "pch\pch.h"
#include "ListBox.h"
#include <Game/System/AbstructInputSystem.h>
#include <Game/Manager/AudioManager.h>

void UI::ListBox::Start()
{
	selectedElementCursor = GetScene().Instantiate<SelectedElementCursor>();
	Get<SelectedElementCursor>(selectedElementCursor).Data().transform.layerOrder = 6;
	Get<SelectedElementCursor>(selectedElementCursor).Data().transform.scale      = Vec2{ 32, 32 };
}

void UI::ListBox::Update()
{
	// パッドからフォーカス操作
	auto uiInput = Get<AbstructInputSystem>().GetUIInput();

	if (uiInput->IsMovedDown())
	{
		MoveFocus(1);
	}
	if (uiInput->IsMovedUp())
	{
		MoveFocus(-1);
	}

	// マウスカーソルからフォーカス操作
	UpdateFocusForMouseCursor();

	// リストボックス内の要素を更新
	for (int i = 0; i < elements.size(); i++)
	{
		Get<ListElement>(elements[i]).Update((i == currentIndex));
	}

	// 選択項目の矢印の位置を更新
	UITransform& elementTransform = Get<UITransform>(elements[currentIndex]);
	UITransform& cursorTransform  = Get<UITransform>(selectedElementCursor);
	// 選択項目からオフセット分離す
	Vec2 offset{ 0.0f, 1.0f };
	cursorTransform.position = elementTransform.position + offset;
}

std::string UI::ListBox::GetSelectionData()
{
	return Get<ListElement>(elements[currentIndex]).Data().value;
}

bool UI::ListBox::IsPushed()
{
	return Get<AbstructInputSystem>().GetPlayerInput()->IsInteractBegin() ||
	       Get<UIRaycastTarget>(elements[currentIndex]).currentState == UIRaycastTargetState_Pressed;
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
	// 移動前の選択位置を保存
	int previousIndex = currentIndex;

	currentIndex += _dir;
	currentIndex = (currentIndex + elements.size()) % elements.size();

	// 選択位置が実際に変わった場合だけSEを再生
	if (currentIndex != previousIndex && !Data().moveSE.IsNull())
	{
		Get<AudioManager>().Play(Data().moveSE);
	}
}

void UI::ListBox::UpdateFocusForMouseCursor()
{
	for (int i = 0; i < elements.size(); i++)
	{
		Get<ListElement>(elements[i]).Update((i == currentIndex));
		UIRaycastTarget& raycastTarget = Get<UIRaycastTarget>(elements[i]);
		if (raycastTarget.currentState == UIRaycastTargetState_Hovered)
		{
			currentIndex = i;
		}
	}
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
