#pragma once
#include <Game/Game.h>
#include "IItemAction.h"
#include "../ItemId.h"

/// @brief アイテムアクション - 弓のデータ
struct ItemActionWeaponBowData
{
	float stringTimeSec{ 0.0f };               ///< 弓を引いている秒数
	float tripleFireTimeThresholdSec{ 1.0f };  ///< 3本発射の切り替え秒数

	const ItemId ARROW_ITEM_ID{ 15 };  ///< 矢のアイテムId
};

/// @brief アイテムアクション - 弓
struct ItemActionWeaponBow : ObjectBase<ItemActionWeaponBowData>, IItemAction
{
	// IItemAction を介して継承されました
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;

	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
};
