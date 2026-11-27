#pragma once
#include <Game/Game.h>

/// @brief AIからの参照用インタフェース：EnemeyController
class IAIReferenceEnemyController
{
  public:
	IAIReferenceEnemyController()          = default;
	virtual ~IAIReferenceEnemyController() = default;

	/// @brief 指定座標に最も近い敵オブジェクトを取得する
	/// @param position 座標
	/// @return 敵オブジェクト
	virtual GameObject GetNearbyEnemyObject(const Vec3 position) = 0;
};
