#pragma once
#include <Game/Game.h>
#include "../ItemId.h"

/// @brief ギフトリクエスト選択するやつのデータ
struct GiftRequestSelectorData
{
	GiftRequestSelectorData(
	    std::vector<ItemId>&& _itemIds) :
	    itemIds{ std::move(_itemIds) }
	{
	}

	Resource<Texture> textureBackground{ "Image/UI/Gift/gift_request_item_background.png" };

	float deadZone{ 0.03f };        ///< 入力を無視するスティックを倒した距離
	std::vector<ItemId> itemIds{};  ///< 選択候補としてのアイテムId
	Vec2 dir{};                     ///< 選択方向

	std::vector<GameObject> objElements{};  ///< 選択候補表示用UIオブジェクト
};

/// @brief ギフトリクエスト選択するやつ
struct GiftRequestSelector : ObjectBase<GiftRequestSelectorData>
{
	using ObjectBase<GiftRequestSelectorData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
	void OnDestroy() override;

	/// @brief 1要素が占める角度(ラジアン)を取得する
	/// @return ラジアン
	float GetElementRangeAngleRad() const;

	/// @brief 試しにギフトリクエストセレクターで選択中のアイテムIdを取得する
	/// @return 選択されていれば
	std::optional<ItemId> TryGetSelectedItemId();

	/// @brief 選択中のindex
	/// @return -1で未選択
	int32_t GetSelectedIndex();
};
