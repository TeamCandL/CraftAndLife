#pragma once
#include <Game/Game.h>
#include "IItemAction.h"
#include "../ItemId.h"

struct ItemActionWeaponsData
{
	const ItemId ITEM_ID_HAMMER{ 5 };      ///< ハンマーのアイテムId
	const ItemId ITEM_ID_AXE{ 6 };         ///< oh-noのアイテムId
	const ItemId ITEM_ID_SWORD_BIG{ 17 };  ///< 大剣のアイテムId

	float standardItemRange{ 3.0f };  ///< アイテムの有効範囲
	float swordBigItemRange{ 5.0f };  ///< 大剣アイテムの有効範囲
};

struct ItemActionWeapons : ObjectBase<ItemActionWeaponsData>, IItemAction
{
	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
};
