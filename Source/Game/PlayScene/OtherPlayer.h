#pragma once
#include "IPlayer.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>
#include "Player/HelthState.h"

struct OtherPlayerData
{
	const Vec3 OBB_OFFSET{ 0.0f, 0.003f, 0.0f };  ///< OBBのオフセット(位置)
	const float OBB_SCALE{ 0.005f };              ///< OBBのスケール

	OtherPlayerData(
	    const std::string& _name) :
	    name{ _name }
	{
		tag.Add("OtherPlayer");
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };

	Component<Name> name{};
	Component<Tag> tag{};
	Component<Transform> transform{ Vec3::Zero(), Vec3::One() * 200.0f };
	Component<StaticOBB> obb{};
	Component<OBBRenderer> obbRenderer{};

	std::array<Resource<ModelFbx>, PlayerRoleType_Max> models{
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-b.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-f.fbx",
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-f.fbx",
	};

	Resource<Shader> shadowShader{ "Shader/ShadowMap.hlsl" };

	PlayerRoleType role{ PlayerRoleType_Healer };

	// ネットワークから受信した目標座標
	Vec3 targetPosition{};

	// 補間用
	float positionInterpolationSpeed = 20.0f;

	// 初回座標受信フラグ
	bool isPositionInitialized = false;

	int32_t animIndex{};
	float animSpeed{ 1.0f };

	// 生存ステート
	bool isDead{ false };

	// HP
	int32_t hp{ 100 };
	int32_t maxHP{ 100 };

	// 死亡アニメーション再生時間
	inline static constexpr float DIE_ANIM_TIME_SEC_MAX{ 19.0f / 60.0f };
	float dieAnimTimeSec{ DIE_ANIM_TIME_SEC_MAX };
};

struct OtherPlayer : ObjectBase<OtherPlayerData>, IPlayer
{
	using ObjectBase<OtherPlayerData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;
	void DrawShadowMap() override;

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
