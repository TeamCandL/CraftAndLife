#pragma once
#include <Game/Game.h>

struct OtherEnemyControllerData
{
	std::unordered_map<int, GameObject> idToObjOtherEnemy{}; ///< ほかの敵オブジェクト
};

/// @brief ほか敵オブジェクトを操作する
struct OtherEnemyController : ObjectBase<OtherEnemyControllerData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
