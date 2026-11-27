#include "pch\pch.h"
#include "GiftRequestSelector.h"
#include <Game/System/AbstructInputSystem.h>
#include "GiftRequestSelectElement.h"
#include "GiftRequestSelectCenterBackground.h"
#include "GiftRequestSelectCenterText.h"
#include "../ItemSystem.h"

void GiftRequestSelector::Start()
{
	objCenterBackground = GetScene().Instantiate<GiftRequestSelectCenterBackground>();
	objCenterText       = GetScene().Instantiate<GiftRequestSelectCenterText>();

	objElements.resize(selectItemInfo.size());
	for (size_t i = 0; i < selectItemInfo.size(); i++)
	{
		objElements[i] = GetScene().Instantiate<GiftRequestSelectElement>(
		    textureBackground,
		    GetElementRangeAngleRad() * static_cast<float>(i) + (GetElementRangeAngleRad() * 0.5f),
		    GetElementRangeAngleRad(),
		    selectItemInfo[i]);
	}
}

void GiftRequestSelector::Update()
{
	if (objElements.size() <= 0)
	{
		Get<GiftRequestSelectCenterText>(objCenterText).SetText("投げ捨てられる\nアイテムがない");
		return;
	}

	const int32_t SELECTED_INDEX = GetSelectedIndex();

	for (size_t i = 0; i < objElements.size(); i++)
	{
		Get<GiftRequestSelectElement>(objElements[i]).SetFocus(i == SELECTED_INDEX);
	}

	if (std::optional<ItemId> itemId = TryGetSelectedItemId())
	{
		const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(*itemId);
		Get<GiftRequestSelectCenterText>(objCenterText)
		    .SetText(std::format("アイテム\n「{}」\nを投げ捨てる", ITEM_INFO._name));
	}
	else
	{
		Get<GiftRequestSelectCenterText>(objCenterText).SetText("");
	}
}

void GiftRequestSelector::Draw() {}

void GiftRequestSelector::OnDestroy()
{
	GetScene().Destroy(objCenterBackground);
	GetScene().Destroy(objCenterText);

	for (auto& obj : objElements)
	{
		GetScene().Destroy(obj);
	}
}

float GiftRequestSelector::GetElementRangeAngleRad() const
{
	return DirectX::XM_2PI / static_cast<float>(selectItemInfo.size());
}

std::optional<ItemId> GiftRequestSelector::TryGetSelectedItemId()
{
	if (objElements.size() <= 0)
	{
		return std::nullopt;
	}

	int32_t selectedIndex = GetSelectedIndex();
	if (selectedIndex < 0)
	{
		return std::nullopt;
	}

	return selectItemInfo[selectedIndex].itemId;
}

int32_t GiftRequestSelector::GetSelectedIndex()
{
	Vec2 currentDir = Get<AbstructInputSystem>().GetPlayerInput()->GetMoveCamera();

	if (currentDir.Length() > deadZone)
	{
		dir = currentDir;
	}

	if (dir.Length() <= deadZone)
	{
		return -1;  // 方向が示されていないなら無視
	}

	// 1つの要素が占める角度(らじあん)
	const float ELEMENT_RANGE_ANGLE_RAD{ GetElementRangeAngleRad() };

	// 選択している方向
	float angleRad = std::atan2f(dir.y, dir.x);

	// 角度ずらして左
	angleRad -= ELEMENT_RANGE_ANGLE_RAD * 0.5f;

	if (angleRad <= 0.0f)
	{
		angleRad += DirectX::XM_2PI;
	}

	const int32_t rawIndex = static_cast<int32_t>((DirectX::XM_2PI - angleRad) / ELEMENT_RANGE_ANGLE_RAD);
	const int32_t maxIndex = static_cast<int32_t>(selectItemInfo.size() - 1);
	return std::clamp(rawIndex, 0, maxIndex);
}
