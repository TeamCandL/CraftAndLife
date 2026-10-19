#pragma once
#include <Game/Game.h>

struct OtherPlayerData
{
};

struct OtherPlayer : ObjectBase<OtherPlayerData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
};
