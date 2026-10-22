#pragma once
#include <Game/Game.h>

struct Player;

struct EnemyAttackCollData
{
	/// @brief 攻撃生成(データ指定なし)	ほぼデバック用
	EnemyAttackCollData() {}
	/// @brief 攻撃生成(データ指定有)
	/// @param position_ 生成者の座標を入れてほしいかも
	/// @param damage_ 生成者のdamageを入れてほしいかも
	/// @param lifeTime_ 任意の寿命を入れてほしいかも
	EnemyAttackCollData(
	    const Vec3 position_,
	    const int32_t damage_,
	    const float lifeTime_) :
	    transform{ position_ },
	    damage{ damage_ },
	    lifeTime{ lifeTime_ }
	{
	}
	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidBody{};      ///< 剛体
	Component<DynamicOBB> obb{};           ///< OBB当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	//-----------------------------------------//
	// パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t damage{};
	float lifeTime{};  ///< 寿命

	const Vec3 OBB_OFFSET{ 0.0f, 0.5f, 0.0f };  ///< OBBのオフセット(位置)
	const float OBB_SCALE{ 1.0f };              ///< OBBのスケール
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