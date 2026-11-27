#pragma once
#include <Game/Game.h>

struct TestPlayerData
{
	GameObject objGiftRequestSelector{};
};

/// @brief テストプレイヤー
struct TestPlayer : ObjectBase<TestPlayerData>
{
	/// @brief 更新処理
	void Update() override;
};
