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
	    itemId{ _itemId },
	    entityId{ _entityId }
	{
	}

	Resource<Shader> mainShader{ "Shader/Entity.hlsl",
		                         RasterizerConfig{ .cullMode = D3D11_CULL_NONE,
		                                           .fillMode = D3D11_FILL_SOLID } };  ///< メインのモデル描画用シェーダ
	Resource<Shader> edgeShader{ "Shader/EntityEdge.hlsl",
		                         RasterizerConfig{ .cullMode = D3D11_CULL_FRONT,
		                                           .fillMode = D3D11_FILL_SOLID } };  ///< エッジ(輪郭)描画用シェーダ
	Resource<Model> model{};                                                          ///< モデル

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidbody{};      ///< 剛体
	Component<DynamicOBB> obb{};           ///< 当たり判定OBB
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	Vec3 velocity{};                       ///< 速度
	ItemId itemId{};                       ///< このエンティティをアイテム化したときのId
	int32_t entityId{};                    ///< エンティティId
	bool picked{ false };                  ///< エンティティが誰かに取得されたか true / false
	bool despawned{ false };               ///< エンティティがデスポーンしたか true /false
	bool isDestroyedOnCollision{ false };  ///< 何かに触れると消されるか？ true / false
	bool canHitToPlayer{ false };          ///< プレイヤーへ当たり判定があるか true / false
	bool canHitToEnemy{ false };           ///< 敵への当たり判定はあるか true / false
	bool canHitToHarvestable{ false };     ///< ハーベスタブルオブジェクトへ当たるか true / false
	bool isThrowingItemEntity{ false };    ///< アイテムとして回収可能な投げ物であるか true / false
	bool isUnderground{ false };           ///< 地面より下にいるか true / false

	float gravityPerSec{ 9.8f };  ///< 重力加速度(秒)
	float friction{ 0.9f };       ///< 摩擦力

	float despawnTimeLeftSec{ 60.0f * 2.0f };                      ///< 死ぬまでの時間(秒)
	float pickupCooldownSec{ 3.0f };                               ///< 拾えない時間(秒)
	inline static const ItemId ITEM_ID_ENEMY_ATTACK_STRING{ 20 };  ///< 敵の攻撃のアイテムId
	inline static const ItemId ITEM_ID_PLAYER_ATTACK_ARROW{ 24 };  ///< プレイヤーの攻撃の矢アイテムId
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

	// IEntity を介して継承されました
	bool IsPicked() const override;
	bool IsDespawned() const override;
	void Despawn() override;
	void Pick() override;
	bool CanPickup() const override;
	bool CanHitToPlayer() const override;
	bool CanHitToEnemy() const override;
	Vec3 GetPosition() const override;
	// IEntity を介して継承されました
	bool IsThrowingItemEntity() const override;

	/// @brief 地面の下にいるか取得
	/// @return
	bool IsUnderground() const override;

	bool CanHitToHarvestable() const override
	{
		return canHitToHarvestable;
	}

	void SetUnderground(
	    const bool _isUnderground) override
	{
		isUnderground = _isUnderground;
	}
};
