#pragma once
#include <Game/Game.h>

struct TestPlayerData
{
	GameObject objGiftRequestBox{};
};

/// @brief テストプレイヤー
struct TestPlayer : ObjectBase<TestPlayerData>
{
	/// @brief 更新処理
	void Update() override;
};
