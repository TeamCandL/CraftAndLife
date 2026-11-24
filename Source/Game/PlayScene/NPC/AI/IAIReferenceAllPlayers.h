#pragma once
#include <Game/Game.h>

/// @brief AIからの参照用インタフェース：Player
class IAIReferenceAllPlayers
{
  public:
	IAIReferenceAllPlayers()          = default;
	virtual ~IAIReferenceAllPlayers() = default;

	/// @brief 指定座標に最も近いプレイヤーオブジェクトを取得する
	/// @param position 座標
	/// @return プレイヤーオブジェクト
	virtual GameObject GetNearbyPlayerObject(const Vec3 position) = 0;
};
