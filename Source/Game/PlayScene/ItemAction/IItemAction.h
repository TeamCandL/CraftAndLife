#pragma once
#include <Game/Game.h>
#include "../ItemId.h"

struct PlayerData;

/// @brief アイテム固有のアクション用インタフェース
class IItemAction
{
  public:
	/// @brief コンストラクタ
	IItemAction() = default;
	/// @brief デストラクタ
	virtual ~IItemAction() = default;

	/// @brief アイテムを使った時の動作
	/// @param _itemId
	/// @param _playerData
	virtual void Action(const ItemId _itemId, PlayerData& _playerData) = 0;
};
