#pragma once
#include "IPlayer.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>

struct OtherPlayerData
{
	OtherPlayerData(
	    const std::string& _name) :
	    name{ _name }
	{
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };

	Component<Name> name{};
	Component<Transform> transform{ Vec3::Zero(), Vec3::One() * 200.0f };

	// Resource<Model> aModel{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-c.fbx" };

	std::array<Resource<Model>, PlayerRoleType_Max> models{
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-b.fbx",  // Healer
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx",    // Tank
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-f.fbx",    // Fighter
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-f.fbx",  // Archer
	};

	PlayerRoleType role{ PlayerRoleType_Healer };

	int32_t animIndex{};

	// 死亡状態
	bool isDead{ false };
};

struct OtherPlayer : ObjectBase<OtherPlayerData>, IPlayer
{
	using ObjectBase<OtherPlayerData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

	Vec3 GetPosition() const override;
	float GetAngleY() const override;

	// 死亡状態を取得
	bool IsDead() const
	{
		return isDead;
	}
};
