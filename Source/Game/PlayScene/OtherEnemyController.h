#pragma once
#include <Game/Game.h>
#include "EnemyType.h"

struct OtherEnemyControllerData
{
	OtherEnemyControllerData(
	    GameObject _objPlaySceneUIController) :
	    objPlaySceneUIController{ _objPlaySceneUIController }
	{
	}

	std::unordered_map<int, GameObject> idToObjOtherEnemy{};  ///< ほかの敵オブジェクト
	GameObject objPlaySceneUIController{};                    ///< UIコントローラーのオブジェクト

	std::vector<GameObject> objEnemyHpBar;
};

/// @brief ほか敵オブジェクトを操作する
struct OtherEnemyController : ObjectBase<OtherEnemyControllerData>
{
	using ObjectBase<OtherEnemyControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	bool IsBossEnemy(
	    EnemyType type)
	{
		return type == EnemyType::BossSkeleton || type == EnemyType::Dragon;
	}
};