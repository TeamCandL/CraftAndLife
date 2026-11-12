#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "EntityNetworkData.h"

class IEntitySystemNetwork
{
  public:
	IEntitySystemNetwork()          = default;
	virtual ~IEntitySystemNetwork() = default;

	/// @brief 通信で受信した実体エンティティを生成する
	/// @param _entityId エンティティID
	/// @param _itemId アイテムID
	/// @param _position 生成位置
	/// @param _velocity 初速度
	virtual void SpawnFromNetwork(
	    const int32_t _entityId,
	    const ItemId _itemId,
	    const Vec3 _position,
	    const Vec3 _velocity) = 0;

	/// @brief ホストであるか設定する
	/// @param _isHost ホストである true / false
	virtual void SetHost(const bool _isHost) = 0;

	/// @brief 指定の実体エンティティを削除する
	/// @param _entityId 指定のエンティティId
	virtual void RemoveFromNetwork(int32_t _entityId) = 0;

	/// @brief スポーンデータを一括取得する
	/// @return スポーンデータ
	virtual std::vector<EntitySpawnData> TakeSpawnsData() = 0;
	/// @brief 拾ったデータを一括取得する
	/// @return 拾ったデータ
	virtual std::vector<EntityPickData> TakePicksData() = 0;
};
