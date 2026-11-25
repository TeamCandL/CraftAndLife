#pragma once
#include <Game/Game.h>
#include "../ItemId.h"

struct GiftRequestSelectorData
{
	GiftRequestSelectorData(
	    std::vector<ItemId>&& _itemIds) :
	    itemIds{ std::move(_itemIds) }
	{
	}

	Resource<Texture> textureBackground{ "Image/UI/Gift/gift_request_item_background.png" };

	std::vector<ItemId> itemIds{};  ///< 選択候補としてのアイテムId
};

struct GiftRequestSelector : ObjectBase<GiftRequestSelectorData>
{
	void Start() override;
	void Update() override;

	/// @brief 試しにギフトリクエストセレクターで選択中のアイテムIdを取得する
	/// @return 選択されていれば
	std::optional<ItemId> TryGetSelectedItemId();
};
