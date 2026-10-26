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

	Resource<Shader> mainShader{ "Shader/Unlit.hlsl" };  ///< メインのモデル描画用シェーダ
	Resource<Shader> edgeShader{ "Shader/Unlit.hlsl" };  ///< エッジ(輪郭)描画用シェーダ
	Resource<Model> model{};                             ///< モデル

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

	void SetPosition(
	    const Vec3 _position)
	{
		transform.position = _position;
	}
	void SetVelocity(
	    const Vec3 _velocity)
	{
		velocity = _velocity;
	}

	/// @brief このエンティティのアイテムIdを取得する
	/// @return アイテムId
	ItemId GetItemId() const override
	{
		return itemId;
	}
};
