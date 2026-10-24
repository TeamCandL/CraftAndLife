#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief エンティティのインタフェース
class IEntity
{
  public:
	/// @brief コンストラクタ
	IEntity() = default;
	/// @brief デストラクタ
	virtual ~IEntity() = default;

	/// @brief アイテムIdを取得する
	/// @return アイテムId
	virtual ItemId GetItemId() const = 0;
};
