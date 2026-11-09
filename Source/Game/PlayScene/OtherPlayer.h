#pragma once
#include "IPlayer.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>
#include "Player/HelthState.h"

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
	float animSpeed{ 1.0f };

	// 生存ステート
	bool isDead{ false };

	// HP
	int32_t hp{ 100 };
	int32_t maxHP{ 100 };

	// 死亡アニメーション再生時間
	inline static constexpr float DIE_ANIM_TIME_SEC_MAX{ 20.0f / 60.0f };
	float dieAnimTimeSec{ DIE_ANIM_TIME_SEC_MAX };
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
