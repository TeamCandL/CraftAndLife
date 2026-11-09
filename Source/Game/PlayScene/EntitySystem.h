#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "IEntitySystemNetwork.h"
#include "EntityNetworkData.h"

struct EntitySystemData
{
	std::vector<GameObject> objEntities{};      ///< エンティティのデータ
	std::vector<EntitySpawnData> spawnsData{};  ///< スポーンデータ
	std::vector<EntityPickData> picksData{};    ///< 拾ったデータ
};

struct EntitySystem : SystemObjectBase<EntitySystem, EntitySystemData>, IEntitySystemNetwork
{
	/// @brief 更新処理
	void Update() override;

	/// @brief エンティティをスポーンさせる
	/// @param _itemId アイテムId
	void Spawn(const ItemId _itemId, const Vec3 _position, const Vec3 _velocity);

	/// @brief エンティティを拾う
	/// @param pos 回収する座標
	/// @param radius 回収可能な半径
	/// @return 拾えたエンティティのGameObject / 拾えなければIsEmptyなGameObject
	GameObject PickNearest(const Vec3 _pos, const float _radius);

	std::vector<EntitySpawnData> TakeSpawnsData() override
	{
		return std::exchange(spawnsData, std::vector<EntitySpawnData>{});
	}

	std::vector<EntityPickData> TakePicksData() override
	{
		return std::exchange(picksData, std::vector<EntityPickData>{});
	}

	/// @brief 通信で受信したエンティティを生成する
	/// @param _entityId エンティティID
	/// @param _itemId アイテムID
	/// @param _position 生成位置
	/// @param _velocity 初速度
	void SpawnFromNetwork(const int32_t _entityId, const ItemId _itemId, const Vec3 _position, const Vec3 _velocity)
	    override;

	/// @brief ホストであるか設定する
	/// @param _isHost ホストである true / false
	void SetHost(const bool _isHost) override;

	/// @brief 指定のエンティティの実体を削除する
	/// @param _entityId エンティティのId
	void RemoveFromNetwork(const int32_t _entityId) override;
};
