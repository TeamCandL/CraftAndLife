#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "EntityNetworkData.h"

/// @brief エンティティ操作のデータ
struct EntitySystemData
{
	/// @brief エンティティのコレクション用情報
	struct EntityInfo
	{
		GameObject obj{};  /// ゲームオブジェクト
		int32_t entityId{};
		float pickupCooldownSec{};       ///< 拾えない時間(秒)
		float despawnTimeLeft{};         ///< 自然消滅までの時間(秒)
		bool isRemoveRequested = false;  ///< 削除要求が済んでるかの判定
	};

	std::vector<EntityInfo> entities{};                 ///< エンティティ管理用
	std::vector<EntitySpawnData> pendingSpawnData{};    ///< ネットワーク送信待ちのスポーンデータ
	std::vector<EntityRemoveData> pendingRemoveData{};  ///< 削除待ちキュー
	int32_t nextEntityId{ 0 };                          ///< 次に割り当てるエンティティID
	bool isHost{ false };
};

/// @brief エンティティ操作
struct EntitySystem : SystemObjectBase<EntitySystem, EntitySystemData>
{
	/// @brief 開始処理
	void Start() override;
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

	/// @brief 通信で受信したエンティティを生成する
	/// @param _entityId エンティティID
	/// @param _itemId アイテムID
	/// @param _position 生成位置
	/// @param _velocity 初速度
	void SpawnFromNetwork(const int32_t _entityId, const ItemId _itemId, const Vec3 _position, const Vec3 _velocity);

	const std::vector<EntitySpawnData>& GetPendingSpawnData() const;

	void ClearPendingSpawnData();

	const std::vector<EntityRemoveData>& GetPendingRemoveData() const;

	void ClearPendingRemoveData();

	void Remove(int32_t _entityId);

	void RemoveFromNetwork(int32_t _entityId);

	void UpdateEntityId(int32_t _oldId, int32_t _newId);

	void SetHost(bool _isHost);
};
