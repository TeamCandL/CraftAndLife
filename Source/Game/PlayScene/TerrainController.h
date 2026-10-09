#pragma once
#include <Game/Game.h>

/// @brief 地形を制御するオブジェクトのデータ
struct TerrainControllerData
{
	GameObject objMainTerrain{};  ///< メインで使う地形
};

/// @brief 地形を制御する
struct TerrainController : ObjectBase<TerrainControllerData>
{
	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;
};
