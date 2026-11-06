#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "IEntity.h"

/// @brief エンティティのデータ
struct EntityData
{
	EntityData(
	    const ItemId _itemId,
	    const int32_t _entityId) :
	    itemId{ _itemId }
	{
	}

	Resource<Shader> mainShader{ "Shader/Unlit.hlsl" };  ///< メインのモデル描画用シェーダ
	Resource<Shader> edgeShader{ "Shader/Unlit.hlsl" };  ///< エッジ(輪郭)描画用シェーダ
	Resource<Model> model{};                             ///< モデル

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidbody{};      ///< 剛体
	Component<DynamicOBB> obb{};           ///< 当たり判定OBB
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	Vec3 velocity{};     ///< 速度
	ItemId itemId{};     ///< このエンティティをアイテム化したときのId
	int32_t entityId{};  ///< エンティティId

	float gravityPerSec{ 9.8f };  ///< 重力加速度(秒)
	float friction{ 0.9f };       ///< 摩擦力
};

/// @brief エンティティ
struct Entity : ObjectBase<EntityData>, IEntity
{
	using ObjectBase<EntityData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

	void SetPosition(
	    const Vec3 _position) override
	{
		transform.position = _position;
	}
	void SetVelocity(const Vec3 _velocity) override;

	/// @brief このエンティティのアイテムIdを取得する
	/// @return アイテムId
	ItemId GetItemId() const override
	{
		return itemId;
	}

	int32_t GetEntityId() const override
	{
		return entityId;
	}
};
