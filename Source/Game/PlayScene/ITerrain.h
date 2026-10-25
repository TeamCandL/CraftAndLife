#pragma once
#include <Game/Game.h>

/// @brief 地形のインタフェース
class ITerrain
{
  public:
	/// @brief コンストラクタ
	ITerrain() = default;
	/// @brief デストラクタ
	virtual ~ITerrain() = default;

	/// @brief 指定点の高さを取得する
	/// @param _worldX 指定点 x
	/// @param _worldZ 指定点 z
	/// @return 高さ y
	virtual float GetHeight(const float _worldX, const float _worldZ) = 0;

	/// @brief 指定点の法線を取得する
	/// @param _worldX 指定点 x
	/// @param _worldZ 指定点 z
	/// @return 法線ベクトル Vec3
	virtual Vec3 GetNormal(const float _worldX, const float _worldZ) = 0;
};
