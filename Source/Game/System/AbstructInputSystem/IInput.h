#pragma once
#include <Game/Game.h>

/// @brief 入力インタフェース
class IInput
{
  public:
	IInput()          = default;
	virtual ~IInput() = default;

	/// @brief 更新処理
	virtual void Update() = 0;
};
