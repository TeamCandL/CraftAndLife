#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief エンティティ操作のデータ
struct EntitySystemData
{
	/// @brief エンティティのコレクション用情報
	struct EntityInfo
	{
		GameObject obj{};           /// ゲームオブジェクト
		float pickupCooldownSec{};  ///< 拾えない時間(秒)
		float despawnTimeLeft{};    ///< 自然消滅までの時間(秒)
	};

	std::vector<GameObject> entities{};  ///< エンティティ管理用
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
};
