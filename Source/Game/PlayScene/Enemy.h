#pragma once
#include "EnemyAiCheckPoint.h"
#include <Game/Game.h>

struct EnemyData
{
	EnemyData()
	{
	}

	enum EnemyType
	{
		Skeleton,
		Spider,
		Bat,
		Dragon,
		EnemyType_Max,
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
	Resource<Shader> shader{"Shader/Unlit.hlsl"}; ///< シェーダ
	// Resource<Model> model{
	//     "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx"}; ///< モデル

	Resource<Model> model{"Model/Enemy/Spider.fbx"};

	/*std::array<Resource<Model>, EnemyType_Max> models{
	    Resource<Model>{ "Model/Enemy/Skeleton.fbx" },
	    Resource<Model>{ "Model/Enemy/Spider.fbx" },
	    Resource<Model>{ "Model/Enemy/Bat.fbx" },
	    Resource<Model>{ "Model/Enemy/Dragon.fbx" },
	};*/

	Component<Transform> transform{}; ///< 座標系
	Component<RigidBody> rigidBody{}; ///< 剛体
	Component<DynamicOBB> obb{};      ///< OBB当たり判定

	float angleX{};
	float angleY{};

	int animIndex{}; ///< アニメーションのインデクス

	//-----------------------------------------//
	// パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t hp{};
	int32_t damage{};

	const int WanRangeW{};
	const int WanRangeH{};

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

	/// @brief 描画処理
	void Draw() override;

	/// 攻撃判定の生成
	void AttackCollider();
	void Damaged(int32_t damage);

	///	徘徊時のチェックポイント設定
	void SetWanderingCheckPoint(const Vec3 &_checkPoint);
};