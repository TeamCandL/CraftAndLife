#pragma once
#include <Game/Game.h>

/// @brief プレイヤー共通のインタフェース
class IPlayer
{
public:
	/// @brief コンストラクタ
	IPlayer() = default;
	/// @brief デストラクタ
	virtual ~IPlayer() = default;

	/// @brief 座標を取得する
	/// @return ワールド座標
	virtual Vec3 GetPosition() const = 0;

	/// @brief Y軸の角度を取得する
	/// @return 角度Y (ラジアン)
	virtual float GetAngleY() const = 0;
};
