#include "HotBar.h"
#include "pch\pch.h"
#include "MiniMapPoint.h"
#include <Game/System/AbstructInputSystem.h>

namespace
{
	constexpr size_t WEAPON_SLOT_INDEX{ 0 };
	constexpr size_t CONSUMABLE_SLOT_INDEX{ 1 };
	constexpr size_t BAG_SLOT_INDEX{ 2 };
}  // namespace

void HotBar::Start()
{
	// 武器
	slots[WEAPON_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::Weapons);
	// 消費アイテム
	slots[CONSUMABLE_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::Consumables);
	// バッグ
	slots[BAG_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::ItemTypeMax);

	for (int i = 0; i < slots.size(); i++)
	{
		SlotData& slotData            = Get<Slot>(slots[i]).Data();
		slotData.transform.layerOrder = 1;
		slotData.transform.scale      = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		slotData.selected             = (currentSlotIdx == i);
		slotData.slotItem             = GetScene().Instantiate<SlotItem>();
		SlotItemData& itemData        = Get<SlotItem>(slotData.slotItem).Data();
		itemData.transform.layerOrder = 5;
		itemData.objItemCount         = GetScene().Instantiate<SlotItemCount>();
		Get<SlotItemCount>(itemData.objItemCount).Data().transform.position = leftSlotItemCountBegin;
		// スタック数が2桁、文字サイズが32と断定
		Get<SlotItemCount>(itemData.objItemCount).Data().transform.scale      = Vec2{ 32 * 2, 32 };
		Get<SlotItemCount>(itemData.objItemCount).Data().transform.layerOrder = 6;
	}
}

void HotBar::Update()
{
	auto playerInput = Get<AbstructInputSystem>().GetPlayerInput();
	if (Get<InputSystem>().GetKeyDown(KeyCode::I) || playerInput->IsMovedHotbarLeft())
	{
		MoveFocusToLeft();
	}
	if (Get<InputSystem>().GetKeyDown(KeyCode::O) || playerInput->IsMovedHotbarRight())
	{
		MoveFocusToRight();
	}

	/// -----------------
	/// スロットのUI更新
	/// -----------------
	for (int i = 0; i < slots.size(); i++)
	{
		SlotData& slotData      = Get<Slot>(slots[i]).Data();
		GameObject& slotGameObj = slots[i];
		// i番目のスロット描画位置のオフセット
		const int offset = (slotMargin + slotEmptyImageExtents) * i;
		// スロットの中心座標
		const Vec2 center = { transform.position.x + leftSlotCenter.x + offset,
			                  transform.position.y + leftSlotCenter.y };
		// スロットが選択されているか判定
		slotData.selected = (currentSlotIdx == i);

		// 選択状態によって画像、座標、サイズを変える
		if (slotData.selected)
		{
			Get<UIImage>(slotGameObj).texture = slotSelectedTexture;
			const int extentsHalf             = slotSelectedImageExtents / 2;
			slotData.transform.position       = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			slotData.transform.scale          = Vec2{ slotSelectedImageExtents, slotSelectedImageExtents };
		}
		else
		{
			Get<UIImage>(slotGameObj).texture = slotEmptyTexture;
			const int extentsHalf             = slotEmptyImageExtents / 2;
			slotData.transform.position       = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			slotData.transform.scale          = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		}

		// NOTE: ゲームループによる更新順番の関係上、ここでワールド行列を計算する必要あり
		Get<Slot>(slotGameObj).CalculateUIWorld();

		SlotItemData& itemData         = Get<SlotItem>(slotData.slotItem).Data();
		Resource<Texture> itemImageRes = Get<ItemSystem>().GetInfo(itemData.id)._imageId;
		itemData.image.texture         = itemImageRes;

		constexpr int itemSize{ 64 };
		const int itemSizeHalf{ itemSize / 2 };
		itemData.transform.position = Vec2{ center.x - itemSizeHalf, center.y - itemSizeHalf };
		itemData.transform.scale    = Vec2{ itemSize, itemSize };

		// スロットのアイテム数のテキスト
		std::string itemCountStr = "";
		ItemType type            = Get<ItemSystem>().GetInfo(itemData.id)._type;
		if (type != ItemType::Weapons && itemData.id != EMPTY_ITEM_ID)
		{
			itemCountStr = std::to_string(itemData.count);
		}
		Get<UITransform>(itemData.objItemCount).position = center;
		Get<UIText>(itemData.objItemCount).content       = itemCountStr;
	}
}

void HotBar::MoveFocusToRight()
{
	MoveFocusImpl(1);
}

void HotBar::MoveFocusToLeft()
{
	MoveFocusImpl(-1);
}

ItemId HotBar::GetSelectedItemId()
{
	return Get<SlotItem>(Get<Slot>(slots[currentSlotIdx]).Data().slotItem).Data().id;
}

bool HotBar::CanRemoveConsumableItem(
    int _count)
{
	SlotItemData& itemData = GetSlotItemData(CONSUMABLE_SLOT_INDEX);
	return (itemData.count >= _count);
}

void HotBar::RemoveConsumableItem(
    int _count)
{
	if (CanRemoveConsumableItem(_count) == false)
		return;
	SlotItemData& itemData = GetSlotItemData(CONSUMABLE_SLOT_INDEX);
	itemData.count -= _count;

	if (itemData.count == 0)
	{
		Get<SlotItem>(Get<Slot>(slots[CONSUMABLE_SLOT_INDEX]).Data().slotItem).Reset();
	}
}

bool HotBar::TrySetItem(
    ItemId _id)
{
	size_t slotIndex = -1;
	ItemInfo info    = Get<ItemSystem>().GetInfo(_id);
	switch (info._type)
	{
	case ItemType::Weapons:
		slotIndex = WEAPON_SLOT_INDEX;
		break;
	case ItemType::Consumables:
		slotIndex = CONSUMABLE_SLOT_INDEX;
		break;
	default:
		break;
	}
	if (slotIndex == -1)
		return false;

	SlotItemData& slotItemData = Get<SlotItem>(Get<Slot>(slots[slotIndex]).Data().slotItem).Data();

	if (slotItemData.id == EMPTY_ITEM_ID || slotItemData.id == _id)
	{
		slotItemData.id = _id;
		slotItemData.count++;
		return true;
	}

	return false;
}

void HotBar::MoveFocusImpl(
    int _dir)
{
	currentSlotIdx += _dir;
	currentSlotIdx = (currentSlotIdx + slots.size()) % slots.size();
}

SlotItemData& HotBar::GetSlotItemData(
    int _idx)
{
	return Get<SlotItem>(Get<Slot>(slots[_idx]).Data().slotItem).Data();
}

void Slot::CalculateUIWorld()
{
	using namespace DirectX;
	transform.uiWorld = transform.uiLocal =
	    XMMatrixIdentity() * DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f) *
	    DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, 0.0f);
}

void SlotItem::Reset()
{
	id                                                   = EMPTY_ITEM_ID;
	image.texture                                        = Get<ItemSystem>().GetInfo(id)._imageId;
	Get<SlotItemCount>(objItemCount).Data().text.content = "";
	count                                                = 0;
}
