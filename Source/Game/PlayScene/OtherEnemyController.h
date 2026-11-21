#pragma once
#include <Game/Game.h>
#include "EnemyType.h"

struct EnemySpawnState
{
	int32_t _id;
	bool _spawned{ false };

	bool IsSpawned()
	{
		return _spawned;
	}
	void Spawned()
	{
		_spawned = true;
	}
};

struct OtherEnemyControllerData
{
	OtherEnemyControllerData(
	    GameObject _objPlaySceneUIController) :
	    objPlaySceneUIController{ _objPlaySceneUIController }
	{
	}

	std::unordered_map<int, GameObject> idToObjOtherEnemy{};       ///< 敵IDと他プレイヤー側の敵オブジェクトの対応表
	std::unordered_map<int, GameObject> idToObjOtherEnemyHpBar{};  ///< 敵IDと対応するHPバーオブジェクトの対応表
	std::vector<EnemySpawnState> enemySpawnList{};                 ///< OtherEnemyが生成済みかの一覧
	GameObject objPlaySceneUIController{};                         ///< プレイシーンのUIコントローラー

	std::vector<GameObject> objOtherEnemyHpBar;
};

/// @brief ほか敵オブジェクトを操作する
struct OtherEnemyController : ObjectBase<OtherEnemyControllerData>
{
	using ObjectBase<OtherEnemyControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	bool IsBossEnemy(const int _id);
};