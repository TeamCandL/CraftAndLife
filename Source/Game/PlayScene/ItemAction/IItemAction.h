#pragma once
#include <Game/Game.h>
#include "../ItemId.h"
#include <Game/System/AbstructInputSystem.h>

struct PlayerData;
struct PlayerHandData;

/// @brief アイテム固有のアクション用インタフェース
class IItemAction
{
  public:
	/// @brief コンストラクタ
	IItemAction() = default;
	/// @brief デストラクタ
	virtual ~IItemAction() = default;

	virtual void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) = 0;

	/// @brief アイテムを使った時の動作
	/// @param _itemId
	/// @param _playerData
	virtual void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) = 0;
};
