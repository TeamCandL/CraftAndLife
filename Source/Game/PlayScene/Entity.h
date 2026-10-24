#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "IEntity.h"

/// @brief エンティティのデータ
struct EntityData
{
	EntityData(
	    const ItemId _itemId) :
	    itemId{ _itemId }
	{
	}

	Resource<Shader> mainShader{};  ///< メインのモデル描画用シェーダ
	Resource<Shader> edgeShader{};  ///< エッジ(輪郭)描画用シェーダ

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidbody{};      ///< 剛体
	Component<DynamicOBB> obb{};           ///< 当たり判定OBB
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	Vec3 velocity{};  ///< 速度
	ItemId itemId{};  ///< このエンティティをアイテム化したときのId
};

/// @brief エンティティ
struct Entity : ObjectBase<EntityData>, IEntity
{
	using ObjectBase<EntityData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

	ItemId GetItemId() const override
	{
		return itemId;
	}
};
