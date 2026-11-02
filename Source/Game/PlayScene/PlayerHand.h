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
	Component<TransformAnimator> transformAnimator{};

	bool isActing{ false };    ///< アイテムを振っている最中か
	bool isOnAction{ false };  ///< アイテムのアイテムのアクションがあった瞬間か true / false

	ItemId haveItemId{};  ///< 持っているアイテムのId

	GameObject objItemAction{};  ///< アイテムアクションのオブジェクト
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

	bool IsActing() const
	{
		return isActing;
	}

	/// @brief アイテムのアクションがあった瞬間か
	/// @return アイテムのアクションがあった瞬間 true / false
	bool IsOnAction() const
	{
		return isOnAction;
	}
};
