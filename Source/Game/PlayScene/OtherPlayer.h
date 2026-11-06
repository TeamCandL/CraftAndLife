
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

	std::array<Resource<Model>, PlayerRoleType_Max> models{
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-b.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-f.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-f.fbx",
	};

	PlayerRoleType role{ PlayerRoleType_Healer };

	int32_t animIndex{};

	// 生存ステート
	bool isDead{ false };

	// 現在HP
	int32_t hp{ 100 };

	// 最大HP
	int32_t maxHP{ 100 };

	int32_t healthState = 0;
	int aliveFlag       = 0;
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

	int32_t GetHP() const
	{
		return Data().hp;
	}

	int32_t GetMaxHP() const
	{
		return Data().maxHP;
	}
};
