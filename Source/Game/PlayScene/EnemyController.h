#pragma once
#include <Game/Game.h>

struct Enemy;

struct EnemyControllerData
{
	std::vector<Enemy> allEnemys;  ///< 全ての敵の情報を格納する
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
	std::vector<Vec3> GetAllEnemyPosition() const;
	std::vector<float> GetAllEnemyAngleY() const;
	std::vector<int32_t> GetAllEnemyId() const;
};