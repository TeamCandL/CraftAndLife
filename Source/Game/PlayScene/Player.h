#pragma once
#include "IPlayer.h"
#include "Player/IPlayerController.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>

/// @brief プレイヤーのデータ
struct PlayerData
{
	PlayerData(
	    const GameObject _objMainCamera) :
	    objMainCamera{ _objMainCamera }
	{
	}

	Resource<Shader> shaderFront{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };
	Resource<Shader> shaderBack{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };

	Resource<Model> model{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-female-a.fbx" };

	Component<Transform> transform{};
	Component<RigidBody> rigidBody{};
	Component<StaticOBB> obb{};
	Component<OBBRenderer> obbRenderer{};
	Component<Name> name{ "Player" };

	GameObject objMainCamera{};
	GameObject objPlayerController{};
	GameObject objPlayerBag{};

	PlayerRoleType role{ PlayerRoleType_Max };

	float angleX{};
	float angleY{};

	int animIndex{};

	// パラメータ
	int32_t maxHP{ 100 };  // 最大HP
	int32_t hp{ 100 };     // 現在HP
	int32_t damage{ 10 };  // 攻撃力

	const float ATTACK_COOL_TIME{ 5.0f };
	float nowAttackCoolTime{ 0.0f };

	const float ATTACK_COLL_LIFE_TIME{ 1.0f };
	const float DAMAGED_COOL_TIME{ 1.0f };
	float nowDamagedCoolTime{ 0.0f };

	const Vec3 OBB_OFFSET{ 0.0f, 0.003f, 0.0f };  ///< OBBのオフセット(位置)
	const float OBB_SCALE{ 0.005f };              ///< OBBのスケール

	// ジャンプ用パラメータ
	float jumpVelocity{ 0.0f };         // 上下方向の速度
	const float JUMP_POWER{ 12.0f };    // ジャンプ初速
	const float GRAVITY{ 15.0f };       // 重力
	const float GROUND_HEIGHT{ 0.0f };  // 地面の高さ
	const float JUMP_CUT_RATE{ 0.5f };  // ボタンを離したときの速度倍率
	bool isJumping{ false };            // ジャンプ中かどうか

	// HP減少テスト用
	float hpTestTimer{ 0.0f };
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>, IPlayer
{
	using ObjectBase<PlayerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	void AttackCollider();

	void Damaged(int32_t damage);

	Vec3 GetPosition() const override;
	float GetAngleY() const override;
};