#include "pch\pch.h"
#include "HotBar.h"
#include "MiniMapPoint.h"
void HotBar::Start()
{
	leftSlot = GetScene().Instantiate<Slot>(slotEmpty);
}

void HotBar::Update()
{
	SlotData& slot                 = Get<Slot>(leftSlot).Data();
	UITransform& leftSlotTransform = Get<UITransform>(leftSlot);
	leftSlotTransform.position     = Vec2{ transform.position.x + leftSlotTop.x, transform.position.y + leftSlotTop.y };
	leftSlotTransform.layerOrder   = 1;
	if (slot.selected)
	{
		leftSlotTransform.scale = Vec2{ slotSelectedImageExtents, slotSelectedImageExtents };
	}
	else
	{
		leftSlotTransform.scale = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
	}

	if (Get<InputSystem>().GetKeyDown(KeyCode::O))
	{
		slot.selected                  = true;
		Get<UIImage>(leftSlot).texture = slotSelected;
	}
	if (Get<InputSystem>().GetKeyDown(KeyCode::K))
	{
		slot.selected                  = false;
		Get<UIImage>(leftSlot).texture = slotEmpty;
	}
}
