#include "HotBar.h"
#include "pch\pch.h"
#include "MiniMapPoint.h"
#include <Game/System/AbstructInputSystem.h>
#include <Game/Manager/AudioManager.h>
#include <ranges>

const unsigned int BIT_FLAG(
    const unsigned int _bit)
{
	return 1 << _bit;
}

namespace
{
	constexpr size_t WEAPON_SLOT_INDEX{ 0 };
	constexpr size_t TOOL_SLOT_INDEX{ 1 };
	constexpr size_t CONSUMABLE_SLOT_INDEX{ 2 };
	constexpr size_t CONSUMABLE_SLOT_SECOND_INDEX{ 3 };
	constexpr size_t BAG_SLOT_INDEX{ 4 };
	constexpr ItemId BAG_ITEM_ID{ 19 };
	bool CannotStack(
	    ItemType _type)
	{
		switch (_type)
		{
		case ItemType::Shield:
		case ItemType::Weapons:
		case ItemType::WeaponBow:
		case ItemType::Tools:
		case ItemType::Backpack:
			return true;
		default:
			return false;
		}
		return false;
	}
}  // namespace

void HotBar::Start()
{
	// 武器
	slots[WEAPON_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture);
	// ツール(斧やハンマー)
	slots[TOOL_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture);
	// 消費アイテム
	slots[CONSUMABLE_SLOT_INDEX]                                   = GetScene().Instantiate<Slot>(slotEmptyTexture);
	slots[CONSUMABLE_SLOT_SECOND_INDEX]                            = GetScene().Instantiate<Slot>(slotEmptyTexture);
	Get<Slot>(slots[CONSUMABLE_SLOT_SECOND_INDEX]).Data().unusable = true;
	// バッグ
	slots[BAG_SLOT_INDEX] = GetScene().Instantiate<Slot>(slotEmptyTexture);

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

	// バッグアイテム作成
	SlotItem& slotItem    = Get<SlotItem>(Get<Slot>(slots[BAG_SLOT_INDEX]).Data().slotItem);
	slotItem.Data().id    = BAG_ITEM_ID;
	slotItem.Data().count = 0;

	Get<Slot>(slots[WEAPON_SLOT_INDEX]).Data().flag =
	    BIT_FLAG(ItemType::Weapons) | BIT_FLAG(ItemType::WeaponBow) | BIT_FLAG(ItemType::Shield);
	Get<Slot>(slots[TOOL_SLOT_INDEX]).Data().flag = BIT_FLAG(ItemType::Tools);
	Get<Slot>(slots[CONSUMABLE_SLOT_INDEX]).Data().flag =
	    BIT_FLAG(ItemType::Arrow) | BIT_FLAG(ItemType::Consumables) | BIT_FLAG(ItemType::Materials);
	Get<Slot>(slots[CONSUMABLE_SLOT_SECOND_INDEX]).Data().flag =
	    BIT_FLAG(ItemType::Arrow) | BIT_FLAG(ItemType::Consumables) | BIT_FLAG(ItemType::Materials);
	Get<Slot>(slots[BAG_SLOT_INDEX]).Data().flag = BIT_FLAG(ItemType::Backpack);
}

void HotBar::Update()
{
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
			// 使用不可の場合のテクスチャを設定
			if (slotData.unusable)
			{
				Get<UIImage>(slotGameObj).texture = slotUnusableTexture;
			}
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

		if (CannotStack(type) == false && itemData.id != EMPTY_ITEM_ID)
		{
			itemCountStr = std::to_string(itemData.count);
		}
		Get<UITransform>(itemData.objItemCount).position = center;
		Get<UIText>(itemData.objItemCount).content       = itemCountStr;
	}
}

void HotBar::UpdateSlot()
{
	auto playerInput = Get<AbstructInputSystem>().GetPlayerInput();
	if (Get<InputSystem>().GetKeyDown(KeyCode::I) || playerInput->IsMovedHotbarLeft())
	{
		MoveFocusToLeft();
		// SEを再生
		Get<AudioManager>().Play(moveHotbarSlotSE);

		OnChangedSelectedItem();
	}
	if (Get<InputSystem>().GetKeyDown(KeyCode::O) || playerInput->IsMovedHotbarRight())
	{
		MoveFocusToRight();
		// SEを再生
		Get<AudioManager>().Play(moveHotbarSlotSE);
		OnChangedSelectedItem();
	}
}

void HotBar::MoveFocusToRight()
{
	MoveFocusToUsableSlot(1);
}

void HotBar::MoveFocusToLeft()
{
	MoveFocusToUsableSlot(-1);
}

ItemId HotBar::GetSelectedItemId()
{
	return Get<SlotItem>(Get<Slot>(slots[currentSlotIdx]).Data().slotItem).Data().id;
}

bool HotBar::CanRemoveSelectedItem(
    int _count)
{
	// バッグは除外不可能
	if (currentSlotIdx == BAG_SLOT_INDEX)
		return false;

	SlotItemData& itemData = GetSlotItemData(currentSlotIdx);
	return (itemData.count >= _count && itemData.id != EMPTY_ITEM_ID);
}

void HotBar::RemoveSelectedItem(
    int _count)
{
	if (CanRemoveSelectedItem(_count) == false)
		return;

	SlotItemData& itemData = GetSlotItemData(currentSlotIdx);
	if (CannotStack(Get<ItemSystem>().GetInfo(itemData.id)._type))
	{
		itemData.count = 0;
	}
	else
	{
		itemData.count -= _count;
	}

	if (itemData.count == 0)
	{
		Get<SlotItem>(Get<Slot>(slots[currentSlotIdx]).Data().slotItem).Reset();
		OnChangedSelectedItem();
	}
}

ItemId HotBar::GetConsumableItemId()
{
	return Get<SlotItem>(Get<Slot>(slots[CONSUMABLE_SLOT_INDEX]).Data().slotItem).Data().id;
}

bool HotBar::CanRemoveConsumableItem(
    int _count)
{
	SlotItemData& itemData = GetSlotItemData(CONSUMABLE_SLOT_INDEX);
	return (itemData.count >= _count && itemData.id != EMPTY_ITEM_ID);
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
		OnChangedSelectedItem();
	}
}

bool HotBar::TrySetItem(
    ItemId _id)
{
	ItemInfo info    = Get<ItemSystem>().GetInfo(_id);
	GameObject slot  = GetSlot(info._type);
	bool cannotStack = CannotStack(info._type);

	if (!slot)
		return false;

	SlotItemData& slotItemData = Get<SlotItem>(Get<Slot>(slot).Data().slotItem).Data();

	if (slotItemData.id == EMPTY_ITEM_ID)
	{
		slotItemData.id = _id;
		slotItemData.count++;
		if (slot == slots[currentSlotIdx])
			OnChangedSelectedItem();
		return true;
	}
	if (slotItemData.id == _id)
	{
		// 武器をすでに持っている場合、追加不可能
		if (cannotStack)
			return false;
		slotItemData.id = _id;
		slotItemData.count++;
		return true;
	}

	return false;
}

bool HotBar::SwapWeaponItem(
    ItemId& _oldItemId,
    ItemId _newItemId)
{
	ItemInfo info = Get<ItemSystem>().GetInfo(_newItemId);
	if (info._type != ItemType::Weapons && info._type != ItemType::WeaponBow && info._type != ItemType::Shield)
	{
		_oldItemId = EMPTY_ITEM_ID;
		return false;
	}
	ItemStack itemStack{};
	bool result = SwapItemImpl(itemStack, _newItemId, WEAPON_SLOT_INDEX);
	_oldItemId  = itemStack.id;
	return result;
}
bool HotBar::SwapConsumableItem(
    ItemStack& _oldItemStack,
    ItemId _newItemId)
{
	ItemInfo info = Get<ItemSystem>().GetInfo(_newItemId);
	if (info._type != ItemType::Consumables && info._type != ItemType::Medicals && info._type != ItemType::Arrow)
	{
		_oldItemStack = ItemStack::EmptyStack();
		return false;
	}

	return SwapItemImpl(_oldItemStack, _newItemId, CONSUMABLE_SLOT_INDEX);
}

bool HotBar::SwapItem(
    ItemStack& _oldItemStack,
    ItemId _newItemId)
{
	ItemInfo info    = Get<ItemSystem>().GetInfo(_newItemId);
	size_t slotIndex = GetSlotIndexFrom(info._type);
	if (slotIndex == -1)
		return false;

	return SwapItemImpl(_oldItemStack, _newItemId, slotIndex);
}

std::vector<ItemStack> HotBar::GetAllSlotItem()
{
	std::vector<ItemStack> itemStacks;
	for (int i = 0; i < slots.size(); i++)
	{
		// バッグは取得させない
		if (i == BAG_SLOT_INDEX)
			continue;
		SlotItem& slotItem = Get<SlotItem>(Get<Slot>(slots[i]).Data().slotItem);
		// 空のアイテムは取得させない
		if (slotItem.Data().id == EMPTY_ITEM_ID || slotItem.Data().count == 0)
			continue;

		itemStacks.emplace_back(slotItem.Data().id, slotItem.Data().count);
	}
	return itemStacks;
}

void HotBar::ClearAllSlotItem()
{
	for (int i = 0; i < slots.size(); i++)
	{
		if (i == BAG_SLOT_INDEX)
			continue;
		Get<SlotItem>(Get<Slot>(slots[i]).Data().slotItem).Reset();
		if (i == currentSlotIdx)
			OnChangedSelectedItem();
	}
}

void HotBar::ExpansionConsumableSlot()
{
	Get<Slot>(slots[CONSUMABLE_SLOT_SECOND_INDEX]).Data().unusable = false;
}

bool HotBar::CanRemoveItem(
    ItemType _itemType,
    int _count)
{
	size_t slotIndex = GetSlotIndexFrom(_itemType);
	if (slotIndex == -1)
		return false;

	SlotItemData& itemData = GetSlotItemData(slotIndex);
	return (itemData.count >= _count && itemData.id != EMPTY_ITEM_ID);
}

void HotBar::RemoveItem(
    ItemType _itemType,
    int _count)
{
	if (CanRemoveItem(_itemType, _count) == false)
		return;

	size_t slotIndex = GetSlotIndexFrom(_itemType);

	if (slotIndex == -1)
		return;

	SlotItemData& itemData = GetSlotItemData(slotIndex);

	if (CannotStack(_itemType))
	{
		itemData.count = 0;
	}
	else
	{
		itemData.count -= _count;
	}
	if (itemData.count == 0)
	{
		Get<SlotItem>(Get<Slot>(slots[slotIndex]).Data().slotItem).Reset();
		if (slotIndex == currentSlotIdx)
			OnChangedSelectedItem();
	}
}

bool HotBar::SwapItemImpl(
    ItemStack& _oldItemStack,
    ItemId _newItemId,
    int _index)
{

	SlotItemData& slotItemData = Get<SlotItem>(Get<Slot>(slots[_index]).Data().slotItem).Data();

	if (slotItemData.id == EMPTY_ITEM_ID)
	{
		_oldItemStack = ItemStack::EmptyStack();
		return false;
	}
	ItemInfo info         = Get<ItemSystem>().GetInfo(slotItemData.id);
	ItemId prevSlotItemId = info._id;

	_oldItemStack.count = slotItemData.count;
	RemoveItem(info._type, slotItemData.count);
	TrySetItem(_newItemId);
	_oldItemStack.id = prevSlotItemId;
	return true;
}

void HotBar::MoveFocusImpl(
    int _dir)
{
	if (_dir == 0)
		return;
	// 移動前の選択スロットを保存
	int previousSlotIdx = currentSlotIdx;

	// 選択スロットを移動
	currentSlotIdx += _dir;
	currentSlotIdx = (currentSlotIdx + slots.size()) % slots.size();
}

SlotItemData& HotBar::GetSlotItemData(
    int _idx)
{
	return Get<SlotItem>(Get<Slot>(slots[_idx]).Data().slotItem).Data();
}

size_t HotBar::GetSlotIndexFrom(
    ItemType _itemType)
{
	size_t slotIndex = -1;
	switch (_itemType)
	{
	case ItemType::Weapons:
	case ItemType::WeaponBow:
	case ItemType::Shield:
		slotIndex = WEAPON_SLOT_INDEX;
		break;
	case ItemType::Consumables:
	case ItemType::Arrow:
	case ItemType::Medicals:
		slotIndex = CONSUMABLE_SLOT_INDEX;
		{
		}
		break;
	case ItemType::Tools:
		slotIndex = TOOL_SLOT_INDEX;
		break;
	case ItemType::Backpack:
		slotIndex = BAG_SLOT_INDEX;
		break;
	default:
		break;
	}
	return slotIndex;
}

GameObject HotBar::GetSlot(
    ItemType _itemType)
{
	auto range = slots | std::views::filter(
	                         [this, _itemType](GameObject& _slot)
	                         {
		                         return Get<Slot>(_slot).Data().flag.test(BIT_FLAG(_itemType));
	                         });

	for (auto& obj : range)
	{
		SlotData& slotData = Get<Slot>(obj).Data();
		if (slotData.unusable == false && Get<SlotItem>(slotData.slotItem).Data().id == EMPTY_ITEM_ID)
		{
			return obj;
		}
	}
	return GameObject{};
}

void HotBar::OnChangedSelectedItem()
{
	if (onChangeSelectedItem)
		onChangeSelectedItem();
}

void HotBar::MoveFocusToUsableSlot(
    int _dir)
{
	for (int i = 0; i < slots.size(); i++)
	{
		MoveFocusImpl(_dir);
		SlotData& slotData = Get<Slot>(slots[currentSlotIdx]).Data();
		if (slotData.unusable == false)
		{
			return;
		}
	}
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
