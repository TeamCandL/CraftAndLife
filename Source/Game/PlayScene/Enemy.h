#pragma once
#include "EnemyAiCheckPoint.h"
#include <Game/Game.h>

/// @brief 敵のパラメータ初期値。Skeleton, Spider, Bat, Dragonの順番でパラメータを格納
namespace EnemyParam
{
	const std::vector<int32_t> Hp{100, 50, 30, 500}; ///< 体力
	const std::vector<int32_t> Damage{10, 5, 3, 50}; ///< 攻撃力
	const std::vector<float> Speed{1.5f, 2.0f, 3.0f, 1.0f}; ///< 移動速度
} // namespace EnemyParam

struct EnemyData
{
	/// @brief 指定なし生成
	EnemyData()
	{
		transform.position = Vec3{0.0f, 0.0f, 0.0f};
		enemyType = EnemyType::Skeleton;

		hp = EnemyParam::Hp[static_cast<int>(EnemyType::Skeleton)];
		damage = EnemyParam::Damage[static_cast<int>(EnemyType::Skeleton)];
		speed = EnemyParam::Speed[static_cast<int>(EnemyType::Skeleton)];
	}
	/// @brief 位置指定生成
	/// @param position_ 位置
	EnemyData(const Vec3 position_) :
	    transform{position_}
	{
		enemyType = EnemyType::Skeleton;

		hp = EnemyParam::Hp[static_cast<int>(EnemyType::Skeleton)];
		damage = EnemyParam::Damage[static_cast<int>(EnemyType::Skeleton)];
		speed = EnemyParam::Speed[static_cast<int>(EnemyType::Skeleton)];
	}
	/// @brief 種類指定生成
	/// @param enemyType_ 敵の種類(Skeleton, Spider, Bat, Dragon)
	EnemyData(const int enemyType_) :
	    enemyType{static_cast<EnemyType>(enemyType_)}
	{
		transform.position = Vec3{0.0f, 0.0f, 0.0f};

		hp = EnemyParam::Hp[enemyType_];
		damage = EnemyParam::Damage[enemyType_];
		speed = EnemyParam::Speed[enemyType_];
	}
	/// @brief 位置、種類指定生成
	/// @param position_ 位置
	/// @param enemyType_ 種類
	EnemyData(const Vec3 position_, int enemyType_ )	:
	    transform{position_},
	    enemyType{static_cast<EnemyType>(enemyType_)}
	{
		hp = EnemyParam::Hp[enemyType_];
		damage = EnemyParam::Damage[enemyType_];
		speed = EnemyParam::Speed[enemyType_];
	}

	/// @brief 敵の種類
	enum EnemyType
	{
		Skeleton = 0,
		Spider,
		Bat,
		Dragon,
		EnemyType_Max,
	};

	/// @brief アニメーションの状態
	///	@param Start アニメーション開始
	/// @param Play アニメーション再生中
	/// @param End アニメーション終了
	enum AnimState
	{
		Start,
		Play,
		End,
	};

	/*Resource<Shader> shaderFront  ///< モデル描画用シェーダー(表面)
	{
	    "Shader/Unlit.hlsl",
	    RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};
	Resource<Shader> shaderBack  ///< モデル描画用シェーダー(裏面)
	{
	    "Shader/Unlit.hlsl",
	    RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};*/
	Resource<Shader> shader{"Shader/Unlit.hlsl", RasterizerConfig{.cullMode = D3D11_CULL_FRONT}}; ///< シェーダ
	// Resource<Model> model{
	//     "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx"}; ///< モデル

	// Resource<Model> model{"Model/Enemy/Bat.fbx"};

	std::array<Resource<Model>, EnemyType_Max> models{
	    Resource<Model>{"Model/Enemy/Skeleton.fbx"},
	    Resource<Model>{"Model/Enemy/Spider.fbx"},
	    Resource<Model>{"Model/Enemy/Bat.fbx"},
	    Resource<Model>{"Model/Enemy/Dragon.fbx"},
	};
	int32_t modelIndex{};

	Component<Transform> transform{Vec3{}, Quaternion::FromEuler(Vec3{-DirectX::XM_PIDIV2, 0.0f, 0.0f})}; ///< 座標系
	Component<RigidBody> rigidBody{};                                                                     ///< 剛体
	Component<DynamicOBB> obb{}; ///< OBB当たり判定

	float angleX{};
	float angleY{};

	int animIndex{}; ///< アニメーションのインデクス

	//-----------------------------------------//
	// パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t hp{};                            ///< 体力
	int32_t damage{};                        ///< 攻撃力
	float speed{};                       ///< 移動速度
	EnemyType enemyType{};                   ///< 敵の種類
	const float ATTACK_COOL_TIME{5.0f};      ///< 攻撃クールタイム
	float nowAttackCoolTime{0.0f};           ///< 現在の攻撃クールタイム
	const float ATTACK_COLL_LIFE_TIME{1.0f}; ///< 攻撃判定の寿命
	const float DAMAGED_COOL_TIME{1.0f};     ///< 被ダメージクールタイム
	float nowDamagedCoolTime{0.0f};          ///< 現在の被ダメージクールタイム

	const int WAN_RANGE{400};               ///< 徘徊範囲
	const int WAN_REFERENCE{WAN_RANGE / 2}; ///< 徘徊基準点,これを現在地(座標)から引いてからランダム値を足すことで、徘徊範囲を決定する

	GameObject objWanderingCheckPoint{}; ///< 徘徊時のチェックポイント
};

/// @brief 敵
struct Enemy : ObjectBase<EnemyData>
{
	using ObjectBase<EnemyData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
	/// @brief 移動処理
	/// @param vecX_ X軸の移動量
	/// @param vecZ_ Z軸の移動量
	/// @param spd_ 速度
	/// @param angleY_	Y軸の回転角度 
	void Move(float vecX_, float vecZ_, float spd_, float angleY_);

	/// @brief 描画処理
	void Draw() override;

	/// @brief 攻撃判定の生成
	void AttackCollider();
	/// @brief
	/// @param damage 受けるダメージ量
	void Damaged(int32_t damage);
	/// @brief アニメーション再生(仮)
	/// @param animIndex_ アニメーション番号
	/// @param state_ アニメーションの状態
	void PlayAnim(int32_t animIndex_, AnimState state_);

	/// @brief 徘徊時のチェックポイント設定
	void SetWanderingCheckPoint(Vec3 position_);
};