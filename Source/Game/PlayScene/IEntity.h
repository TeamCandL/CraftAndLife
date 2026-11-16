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

	/// @brief エンティティが取得されたかを取得する
	/// @return 取得された
	virtual bool IsPicked() const = 0;
	/// @brief エンティティを取得する
	virtual void Pick() = 0;

	/// @brief エンティティをデスポーンさせる NOTE: サーバーへの送信はない
	virtual void Despawn() = 0;

	/// @brief エンティティの座標をセットする
	/// @param _position 座標
	virtual void SetPosition(const Vec3 _position) = 0;
	/// @brief エンティティの速度をセットする
	/// @param _velocity 速度
	virtual void SetVelocity(const Vec3 _velocity) = 0;

	/// @brief 拾えますか？
	/// @return 拾えます！true / false
	virtual bool CanPickup() const = 0;
	/// @brief デスポーンしていますか？
	/// @return デスポーンしています true / false
	virtual bool IsDespawned() const = 0;

	/// @brief プレイヤーへ当たりますか？
	/// @return 当たります true / false
	virtual bool CanHitToPlayer() const = 0;
	/// @brief 敵へ当たりますか？
	/// @return 当たります true / false
	virtual bool CanHitToEnemy() const = 0;
	/// @brief 座標を取得する
	/// @return ワールド座標 (x,y,z)
	virtual Vec3 GetPosition() const = 0;

	/// @brief 投げて回収可能なエンティティか
	/// @return true / false
	virtual bool IsThrowingItemEntity() const = 0;

	/// @brief 地面の下にいるか
	/// @return true / false
	virtual bool IsUnderground() const                     = 0;
	virtual bool CanHitToHarvestable() const               = 0;
	virtual void SetUnderground(const bool _isUnderground) = 0;
};
