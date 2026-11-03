#pragma once
#include <Game/Game.h>
#include "EnemyStatus.h"

struct EnemyControllerData
{
	std::vector<GameObject> allEnemys;  ///< 全ての敵の情報を格納する
	int32_t nextEnemyId = 0;
};

struct EnemyController : ObjectBase<EnemyControllerData>
{
	using ObjectBase<EnemyControllerData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	void AddEnemy(const GameObject _enemy);

	/// @brief 全ての敵の座標を取得する
	/// @return 座標一覧を返す
	std::vector<Vec3> GetAllEnemyPosition();
	std::vector<float> GetAllEnemyAngleY();
	std::vector<int32_t> GetAllEnemyId();
	std::vector<EnemyStatus> GetAllEnemyStatus();

	/// @brief IDを指定して敵にダメージを与える
	/// @param enemyId ダメージを受ける敵のID
	/// @param damage ダメージ量
	void DamagedEnemy(int32_t enemyId, int32_t damage);
};