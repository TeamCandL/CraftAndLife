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

	static auto normalizeAngle = [](const float _angle)
	{
		if (_angle <= 0.0f)
		{
			return _angle + DirectX::XM_2PI;
		}
		else
		{
			return _angle;
		}
	};

	objElements.resize(itemIds.size());
	for (size_t i = 0; i < itemIds.size(); i++)
	{
		objElements[i] = GetScene().Instantiate<GiftRequestSelectElement>(
		    textureBackground,
		    normalizeAngle(GetElementRangeAngleRad() * static_cast<float>(i) + (GetElementRangeAngleRad() * 0.5f)),
		    normalizeAngle(GetElementRangeAngleRad() * static_cast<float>(i + 1) + (GetElementRangeAngleRad() * 0.5f)),
		    itemIds[i]);
	}
}

void GiftRequestSelector::Update()
{
	const int32_t SELECTED_INDEX = GetSelectedIndex();
	for (size_t i = 0; i < objElements.size(); i++)
	{
		Get<GiftRequestSelectElement>(objElements[i]).SetFocus(i == SELECTED_INDEX);
	}

	if (std::optional<ItemId> itemId = TryGetSelectedItemId())
	{
		const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(*itemId);
		Get<GiftRequestSelectCenterText>(objCenterText)
		    .SetText(std::format("「{}」\nが欲しい！\nと叫ぶ", ITEM_INFO._name));
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
		if (!obj.IsEmpty())
		{
			continue;
		}
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

	ImGui::Begin("SelectedIndex");
	ImGui::Text("%3.2f", angleRad);
	ImGui::End();

	return static_cast<int32_t>((DirectX::XM_2PI - angleRad) / ELEMENT_RANGE_ANGLE_RAD);
}
