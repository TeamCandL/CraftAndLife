#include "HotBar.h"
#include "pch\pch.h"
#include "MiniMapPoint.h"
#include <Game/System/AbstructInputSystem.h>

void HotBar::Start()
{
	// 武器
	slots[0] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::Weapons);
	// 消費アイテム
	slots[1] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::Consumables);
	// バッグ
	slots[2] = GetScene().Instantiate<Slot>(slotEmptyTexture, ItemType::Weapons);

	for (int i = 0; i < slots.size(); i++)
	{
		SlotData& slotData            = Get<Slot>(slots[i]).Data();
		slotData.transform.layerOrder = 1;
		slotData.transform.scale      = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		slotData.selected             = (currentSlotIdx == i);
		slotData.slotItem             = GetScene().Instantiate<SlotItem>();
		SlotItemData& itemData        = Get<SlotItem>(slotData.slotItem).Data();
		itemData.transform.layerOrder = 5;
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

void HotBar::MoveFocusImpl(
    int _dir)
{
	currentSlotIdx += _dir;
	currentSlotIdx = (currentSlotIdx + slots.size()) % slots.size();
}

void Slot::CalculateUIWorld()
{
	using namespace DirectX;
	transform.uiWorld = transform.uiLocal =
	    XMMatrixIdentity() * DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f) *
	    DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, 0.0f);
}
