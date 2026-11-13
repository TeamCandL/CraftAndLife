#pragma once
#include <Game/Game.h>

struct Enemy;

struct PlayerAttackCollData
{
	PlayerAttackCollData() {}

	PlayerAttackCollData(
	    const Vec3 position_,
	    const int32_t damage_,
	    const float lifeTime_,
	    const bool isGreatsword_) :
	    transform{ position_ },
	    damage{ damage_ },
	    lifeTime{ lifeTime_ },
	    isGreatsword{ isGreatsword_ }
	{
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl" };
	Resource<Model> model{ "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx" };

	Component<Transform> transform{};
	Component<RigidBody> rigidBody{};
	Component<DynamicOBB> obb{};

	float angleX{};
	float angleY{};

	int animIndex{};

	//-----------------------------------------//
	// パラメータ
	//-----------------------------------------//
	int32_t damage{};
	float lifeTime{};

	// 大剣による攻撃かどうか
	bool isGreatsword{};
};

/// @brief プレイヤーの攻撃当たり判定
struct PlayerAttackColl : ObjectBase<PlayerAttackCollData>
{
	using ObjectBase<PlayerAttackCollData>::ObjectBase;

	void Start() override;
	void Update() override;

	void Move();

	/// 対象へのダメージ付与
	void SendDamage(Enemy& enemy, int32_t damage);
};