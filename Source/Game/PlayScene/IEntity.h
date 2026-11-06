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
	/// @brief エンティティIdを取得する
	/// @return エンティティId
	virtual int32_t GetEntityId() const = 0;

	/// @brief エンティティの座標をセットする
	/// @param _position 座標
	virtual void SetPosition(const Vec3 _position) = 0;
	/// @brief エンティティの速度をセットする
	/// @param _velocity 速度
	virtual void SetVelocity(const Vec3 _velocity) = 0;
};
