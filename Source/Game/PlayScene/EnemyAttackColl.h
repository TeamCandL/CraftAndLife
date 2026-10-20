#pragma once
#include <Game/Game.h>

struct Player;

struct EnemyAttackCollData
{
	EnemyAttackCollData()
	{
	}
	EnemyAttackCollData(const Vec3 position_) :
		transform{position_}
	{
	}

	Resource<Shader> shader{"Shader/Unlit.hlsl"}; ///< シェーダ
	Resource<Model> model{
	    "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx"}; ///< モデル

	Component<Transform> transform{}; ///< 座標系
	Component<RigidBody> rigidBody{}; ///< 剛体
	Component<DynamicOBB> obb{};      ///< OBB当たり判定

	float angleX{};
	float angleY{};

	int animIndex{}; ///< アニメーションのインデクス

	//-----------------------------------------//
	// パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t damage{};
};

/// @brief 敵の攻撃当たり判定
struct EnemyAttackColl : ObjectBase<EnemyAttackCollData>
{
	using ObjectBase<EnemyAttackCollData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// 対象へのダメージ付与
	void SendDamage(Player& player, int32_t damage_);
};