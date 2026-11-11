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

	/// @brief 地形の高さを取得する
	/// @param _worldX ワールド座標x
	/// @param _worldZ ワールド座標z
	/// @return ワールド座標y
	float GetHeight(const float _worldX, const float _worldZ);

	/// @brief 指定の座標を地形の範囲内へ変換する
	/// @param _position 指定の座標
	/// @param _marginWidth マージン幅
	/// @return 地形の範囲内の座標
	Vec3 ToSafePosition(const Vec3 _position, const Vec3 _marginWidth);
};
