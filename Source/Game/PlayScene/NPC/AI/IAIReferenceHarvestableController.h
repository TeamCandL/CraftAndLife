#pragma once
#include <Game/Game.h>

/// @brief AIからの参照用インタフェース：HarvestableController
class IAIReferenceHarvestableController
{
  public:
	IAIReferenceHarvestableController()          = default;
	virtual ~IAIReferenceHarvestableController() = default;

	/// @brief 指定座標に最も近いハーベスタブルオブジェクトを取得する
	/// @param position 座標
	/// @return ハーベスタブルオブジェクト
	virtual GameObject GetNearbyHarvestableObject(const Vec3 position) = 0;
};
