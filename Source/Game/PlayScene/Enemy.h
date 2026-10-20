#pragma once
#include "EnemyAiCheckPoint.h"
#include <Game/Game.h>

struct EnemyData
{
	EnemyData()
	{
	}
	EnemyData(const Vec3 position_) :
	    transform{position_}
	{
	}

	/// @brief 敵の種類
	enum EnemyType
	{
		Skeleton,
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
	float speed{5.0f};                       ///< 移動速度
	const float ATTACK_COOL_TIME{5.0f};      ///< 攻撃クールタイム
	float nowAttackCoolTime{0.0f};           ///< 現在の攻撃クールタイム
	const float ATTACK_COLL_LIFE_TIME{1.0f}; ///< 攻撃判定の寿命
	const float DAMAGED_COOL_TIME{1.0f};     ///< 被ダメージクールタイム
	float nowDamagedCoolTime{0.0f};          ///< 現在の被ダメージクールタイム

	const int WAN_RANGE{100};               ///< 徘徊範囲
	const int WAN_REFERENCE{WAN_RANGE / 2}; ///< 徘徊基準点

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
	void Move(float vecX_, float vecZ_, float spd_);

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
	///	@param _checkPoint 設定するチェックポイントの座標
	void SetWanderingCheckPoint(const Vec3& _checkPoint);
};