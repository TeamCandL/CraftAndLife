#pragma once
#include <Game/Game.h>
#include "IItemAction.h"

struct ItemActionWeaponBow : ObjectBase<>, IItemAction
{
	// IItemAction を介して継承されました
	void Action(const ItemId _itemId, PlayerData& _playerData) override;
};
