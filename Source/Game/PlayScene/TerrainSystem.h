#pragma once
#include <Game/Game.h>

/// @brief 地形システムのデータ
struct TerrainSystemData
{
	GameObject objMainTerrain{};  ///< メインの地形オブジェクト
};

/// @brief 地形システム
struct TerrainSystem : SystemObjectBase<TerrainSystem, TerrainSystemData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 地形を登録する
	/// @param _objTerrain 地形オブジェクト
	void Register(const GameObject _objTerrain);
};
