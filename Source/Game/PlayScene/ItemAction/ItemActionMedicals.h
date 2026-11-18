#pragma once
#include <Game/Game.h>
#include "IItemAction.h"

struct ItemActionMedicalsData
{
	const float ACTION_RANGE{ 3.0f };       ///< 医療系アクションが届く範囲
	GameObject objHealTargetOtherPlayer{};  ///< 治療中の他プレイヤーオブジェクト
	const float HEAL_TIME{ 5.0f };          ///< ヒール時間(秒)
	float healTimeLeftSec{};                ///< ヒール完了までの時間(秒)
	const float RESPAWN_HEALTH{ 100 };      ///< 復活したときの体力値

	// 蘇生中のSE
	Resource<ResourceType::Audio> resuscitationSE{ "BGM_SE/Process Of Resuscitation.mp3" };
	GameObject objHealingEffect{};  ///< 治療中エフェクトのオブジェクト
};

struct ItemActionMedicals : ObjectBase<ItemActionMedicalsData>, IItemAction
{
	void Start() override;
	// IItemAction を介して継承されました
	void Action(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
	// IItemAction を介して継承されました
	void Update(const ItemId _itemId, PlayerData& _playerData, PlayerHandData& _playerHandData) override;
	void OnDestroy() override;
};
