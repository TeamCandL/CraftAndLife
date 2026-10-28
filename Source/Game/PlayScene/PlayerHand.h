#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief プレイヤーの手持ちのデータ
struct PlayerHandData
{
	PlayerHandData(
	    const GameObject _objPlayer) :
	    transform{}
	{
	}

	Component<Transform> transform{};
};

/// @brief プレイヤーの手持ち
struct PlayerHand : ObjectBase<PlayerHandData>
{
	/// @brief 更新処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;

	/// @brief アイテムをセットする
	/// @param アイテムのId
	void SetItem(const ItemId);
};
