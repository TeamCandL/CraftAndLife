#pragma once
#include <Game/Game.h>
#include "IItemAction.h"
#include "../ItemId.h"

struct ItemActionWeaponsData
{
	const ItemId ITEM_ID_HAMMER{ 5 };  ///< ハンマーのアイテムId
	const ItemId ITEM_ID_AXE{ 7 };     ///< oh-noのアイテムId
};

struct ItemActionWeapons : ObjectBase<ItemActionWeaponsData>, IItemAction
{
	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
};
