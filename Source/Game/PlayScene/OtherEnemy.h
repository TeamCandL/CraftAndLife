#pragma once
#include "IPlayer.h"
#include "EnemyType.h"
#include <Game/Game.h>

struct OtherEnemyData
{
	OtherEnemyData(
	    int _enemyId,
	    EnemyType _enemyType) :
	    enemyId{ _enemyId },
	    enemyType{ _enemyType }
	{
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ

	int enemyId{};          ///< 敵ID
	EnemyType enemyType{};  ///< 今の敵の種類

	std::array<Resource<Model>, EnemyType_Max> models{
		Resource<Model>{ "Model/Enemy/Skeleton.fbx" }, Resource<Model>{ "Model/Enemy/Skeleton.fbx" },
		Resource<Model>{ "Model/Enemy/Spider.fbx" },   Resource<Model>{ "Model/Enemy/Bat.fbx" },
		Resource<Model>{ "Model/Enemy/Dragon.fbx" },
	};

	Component<Transform> transform{ Vec3{}, Quaternion::FromEuler(Vec3{ 90.0f, 0.0f, 0.0f }) };  ///< 座標系

	int32_t animIndex{};

	Component<RigidBody> rigidBody{};
	Component<StaticOBB> obb{};

	// 受けたダメージ量
	int32_t damage{};
	bool damagedFlag{ false };
};

struct OtherEnemy : ObjectBase<OtherEnemyData>
{
	using ObjectBase<OtherEnemyData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;

	/// @brief ホストへ敵の被ダメージを通知
	void SendEnemyDamage(int32_t enemyId, int32_t damage);
};
