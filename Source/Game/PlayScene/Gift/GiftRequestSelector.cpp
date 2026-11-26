#include "pch\pch.h"
#include "GiftRequestSelector.h"
#include <Game/System/AbstructInputSystem.h>
#include "GiftRequestSelectElement.h"

void GiftRequestSelector::Start()
{
	objElements.resize(itemIds.size());
	for (size_t i = 0; i < itemIds.size(); i++)
	{
		objElements[i] = GetScene().Instantiate<GiftRequestSelectElement>(
		    textureBackground,
		    GetElementRangeAngleRad() * static_cast<float>(i),
		    GetElementRangeAngleRad() * static_cast<float>(i + 1));
	}
}

void GiftRequestSelector::Update()
{
	const size_t SELECTED_INDEX = GetSelectedIndex();
	for (size_t i = 0; i < objElements.size(); i++)
	{
		Get<GiftRequestSelectElement>(objElements[i]).SetFocus(i == SELECTED_INDEX);
	}
}

void GiftRequestSelector::Draw() {}

void GiftRequestSelector::OnDestroy()
{
	for (auto& obj : objElements)
	{
		GetScene().Destroy(obj);
	}
}

float GiftRequestSelector::GetElementRangeAngleRad() const
{
	return DirectX::XM_2PI / static_cast<float>(itemIds.size());
}

std::optional<ItemId> GiftRequestSelector::TryGetSelectedItemId()
{
	int32_t selectedIndex = GetSelectedIndex();
	if (selectedIndex < 0)
	{
		return std::nullopt;
	}

	return itemIds[selectedIndex];
}

int32_t GiftRequestSelector::GetSelectedIndex()
{
	dir = Get<AbstructInputSystem>().GetPlayerInput()->GetMoveCamera();

	if (dir.Length() <= deadZone)
	{
		return -1;  // 方向が示されていないなら無視
	}

	// 選択している方向
	const float ANGLE_RAD = std::atan2f(dir.y, dir.x);

	// 1つの要素が占める角度(らじあん)
	const float ELEMENT_RANGE_ANGLE_RAD{ GetElementRangeAngleRad() };

	return static_cast<int32_t>(ANGLE_RAD / ELEMENT_RANGE_ANGLE_RAD);
}
