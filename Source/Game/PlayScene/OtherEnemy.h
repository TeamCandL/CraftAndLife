#pragma once
#include "IPlayer.h"
#include "EnemyType.h"
#include <Game/Game.h>

struct StateRegistry;

struct OtherEnemyData
{
	OtherEnemyData(
	    int _enemyId,
	    EnemyType _enemyType) :
	    enemyId{ _enemyId },
	    enemyType{ _enemyType }

	{
	}

	// OtherEnemyData 内に追加
	bool isBoss{ false };  // 必要に応じて同期・判定用に追加

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ

	int enemyId{};          ///< 敵ID
	EnemyType enemyType{};  ///< 今の敵の種類

	std::array<Resource<Model>, EnemyType_Max> models{
		Resource<Model>{ "Model/Enemy/Skeleton.fbx" }, Resource<Model>{ "Model/Enemy/Skeleton.fbx" },
		Resource<Model>{ "Model/Enemy/Spider.fbx" },   Resource<Model>{ "Model/Enemy/Bat.fbx" },
		Resource<Model>{ "Model/Enemy/Dragon.fbx" },
	};

	Resource<Shader> shadowShader{ "Shader/ShadowMap.hlsl" };

	Component<Transform> transform{ Vec3{}, Quaternion::FromEuler(Vec3{ 90.0f, 0.0f, 0.0f }) };  ///< 座標系

	Vec3 targetPosition{};
	Vec3 previousPosition{};
	Quaternion previousRotation{};
	bool isPositionInitialized = false;

	int32_t animIndex{};

	Component<RigidBody> rigidBody{};
	Component<StaticOBB> obb{};

	// 受けたダメージ量
	int32_t damage{};
	bool damagedFlag{ false };

	// 死亡演出関連
	bool isDead{ false };                      // 死亡演出中かどうか
	float deathElapsedSec{ 0.0f };             // 死亡演出の経過時間
	float fallDurationSec{ 0.6f };             // 横たわるまでの時間
	float shrinkDurationSec{ 0.8f };           // 縮小して消滅するまでの時間
	Vec3 deathStartScale{ 1.0f, 1.0f, 1.0f };  // 死亡演出開始時にスケール
	Quaternion deathStartRotation{};           // 死亡演出開始時の回転
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

	void DrawShadowMap() override;

	/// @brief ホストへ敵の被ダメージを通知
	void SendEnemyDamage(int32_t enemyId, int32_t damage);

	void StartDeathAnimation();

	void UpdateDeathAnimation();

	void RegisterStates(StateRegistry& registry) const;
};
