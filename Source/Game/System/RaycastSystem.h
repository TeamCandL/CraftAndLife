#pragma once
#include <Game/Game.h>
#include "RaycastSystem/RaycastHitInfo.h"

/// @brief レイキャストシステムのデータ
struct RaycastSystemData
{
	std::vector<DirectX::BoundingOrientedBox> directXOBBs{};
};

/// @brief レイキャストシステム
struct RaycastSystem : SystemObjectBase<RaycastSystem, RaycastSystemData>
{
	/// @brief OBBへのレイキャストを行う
	/// @param _origin 始点
	/// @param _direction 方向(単位ベクトル)
	/// @param _length 距離
	/// @param _pOut 当たった詳細情報のポインタ渡し (nullptr可能)
	/// @return あたったか
	bool Raycast(const Vec3 _origin, const Vec3 _direction, const float _length, RaycastHitInfo* _pOut = nullptr);

	/// @brief OBBへのレイキャストを行い、線分に当たったすべての結果を返す 計算量はRaycastと変わらない
	/// @param _origin 始点
	/// @param _direction 方向(単位ベクトル)
	/// @param _length 距離
	/// @return 線分にあたったすべての情報
	std::vector<RaycastHitInfo> RaycastAll(const Vec3 _origin, const Vec3 _direction, const float _length);

	DirectX::BoundingOrientedBox ConvertToDirectXOBB(const StaticOBB& _obb, const Transform& _transform);
};
