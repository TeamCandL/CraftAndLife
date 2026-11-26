#include "pch\pch.h"
#include "GiftRequestSelectElement.h"
#include "GiftRequestSelectElementIcon.h"
#include "GiftRequestSelectElementText.h"
#include "../ItemSystem.h"

void GiftRequestSelectElement::Start()
{
	const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(itemId);

	const float ICON_MID_RADIUS{ image.minRadius + ((image.maxRadius - image.minRadius) * 0.5f) };
	const float ICON_CENTER_ANGLE_RAD{ image.beginAngleRad - ((image.endAngleRad - image.beginAngleRad) * 0.5f) };

	const Vec2 CENTER_POSITION = transform.position + transform.scale * 0.5f;

	objIcon = GetScene().Instantiate<GiftRequestSelectElementIcon>(
	    ITEM_INFO._imageId,
	    CENTER_POSITION + Vec2{ std::cosf(ICON_CENTER_ANGLE_RAD) * ICON_MID_RADIUS,
	                            std::sinf(ICON_CENTER_ANGLE_RAD) * ICON_MID_RADIUS });
	objIcon = GetScene().Instantiate<GiftRequestSelectElementText>(
	    std::format("x{}", count),
	    CENTER_POSITION + Vec2{ std::cosf(ICON_CENTER_ANGLE_RAD) * ICON_MID_RADIUS,
	                            std::sinf(ICON_CENTER_ANGLE_RAD) * ICON_MID_RADIUS });
}

void GiftRequestSelectElement::Update() {}

void GiftRequestSelectElement::OnDestroy()
{
	GetScene().Destroy(objIcon);
}

void GiftRequestSelectElement::SetFocus(
    const bool _focused)
{
	if (_focused)
	{
		image.addColor = Color{ 0.1f, 0.1f, 0.1f, 0.0f };  // ちょっと明るくする
	}
	else
	{
		image.addColor = Color::CLEAR;
	}
}
