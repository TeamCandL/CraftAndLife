#pragma once
#include <Game/Game.h>

struct PlayerData;

/// @brief プレイヤー固有の操作
class IPlayerJobController
{
  public:
	/// @brief コンストラクタ
	IPlayerJobController() = default;
	/// @brief デストラクタ
	virtual ~IPlayerJobController() = default;

	/// @brief 更新処理
	virtual void Update(PlayerData& _data) = 0;
};
