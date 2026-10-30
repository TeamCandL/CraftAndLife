#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief プレイヤーの手持ちのデータ
struct PlayerHandData
{
	PlayerHandData(
	    const GameObject _objPlayer) :
	    transform{ _objPlayer }
	{
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl" };

	Component<Transform> transform{};

	ItemId haveItemId{ 1 };
};

/// @brief プレイヤーの手持ち
struct PlayerHand : ObjectBase<PlayerHandData>
{
	using ObjectBase<PlayerHandData>::ObjectBase;

	/// @brief 更新処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;

	/// @brief アイテムをセットする
	/// @param アイテムのId
	void SetItem(const ItemId _itemId);
};
