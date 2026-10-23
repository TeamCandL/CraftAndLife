#pragma once
#include <Game/Game.h>

struct EntityData
{
	Component<Transform> transform{};
	Component<RigidBody> rigidbody{};
};

struct Entity : ObjectBase<EntityData>
{
	void Start() override;
	void Update() override;
	void Draw() override;
};
