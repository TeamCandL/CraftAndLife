#pragma once
#include <Game/Game.h>
#include "IItemAction.h"
#include "../ItemId.h"

struct ItemActionConsumables : ObjectBase<>, IItemAction
{
	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
};
