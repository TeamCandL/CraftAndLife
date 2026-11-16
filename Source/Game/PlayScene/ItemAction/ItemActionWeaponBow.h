#pragma once
#include <Game/Game.h>
#include "IItemAction.h"
#include "../ItemId.h"

/// @brief アイテムアクション - 弓のデータ
struct ItemActionWeaponBowData
{
	float stringTimeSec{ 0.0f };               ///< 弓を引いている秒数
	float tripleFireTimeThresholdSec{ 1.0f };  ///< 3本発射の切り替え秒数

	const ItemId ITEM_ID_PLAYER_ATTACK_ARROW{ 24 };  ///< 矢のアイテムId
	const float FIRE_SPEED_PER_SEC{ 30.0f };         ///< 矢の初速
	const float TRIPLE_FIRE_DISTANCEX{ 2.0f };       ///< 3本射出するときの真ん中の矢からの左右の幅

	// 弓を引き絞る音
	Resource<Audio> pullBowSE{ "BGM_SE/Pull the bow.mp3" };

	// 弓矢を放ったときのSE
	Resource<Audio> shootArrowSE{ "BGM_SE/Shoot an arrow.mp3" };
};

/// @brief アイテムアクション - 弓
struct ItemActionWeaponBow : ObjectBase<ItemActionWeaponBowData>, IItemAction
{
	// IItemAction を介して継承されました
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;

	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
};
