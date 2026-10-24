#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief エンティティのデータ
struct EntityData
{
	Resource<Shader> shader{};

	Component<Transform> transform{};
	Component<RigidBody> rigidbody{};
};

/// @brief エンティティ
struct Entity : ObjectBase<EntityData>
{
	void Start() override;
	void Update() override;
	void Draw() override;
};
