#include "pch\pch.h"
#include "ItemActionWeaponBow.h"
#include "../Player.h"
#include "../PlayerHand.h"
#include "../EntitySystem.h"
#include "../MainCamera.h"
#include "../UI/HotBar.h"
#include <Game/Manager/AudioManager.h>

/// @brief 弓を発射する
void ItemActionWeaponBow::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	EntitySystem& entitySystem = Get<EntitySystem>();
	Vec3 firePositionFrom      = _playerHandData.transform.WorldPosition();
	Vec3 firePositionTo        = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();
	HotBar& hotBar             = Get<HotBar>(_playerData.objHotbar);

	if (stringTimeSec >= tripleFireTimeThresholdSec)
	{
		Vec3 right = _playerData.transform.Right();

		// 矢を1本以上発射できるか確認
		if (!hotBar.CanRemoveConsumableItem())
			return;
		hotBar.RemoveConsumableItem();

		// 中央の矢
		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom).Normalize() * FIRE_SPEED_PER_SEC);

		// 矢の発射音
		Get<AudioManager>().Play(shootArrowSE);

		// 左側の矢
		if (!hotBar.CanRemoveConsumableItem())
			return;
		hotBar.RemoveConsumableItem();

		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom + right * -TRIPLE_FIRE_DISTANCEX).Normalize() * FIRE_SPEED_PER_SEC);

		// 右側の矢
		if (!hotBar.CanRemoveConsumableItem())
			return;
		hotBar.RemoveConsumableItem();

		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom + right * TRIPLE_FIRE_DISTANCEX).Normalize() * FIRE_SPEED_PER_SEC);

		VCU_LOG("3本");
	}
	else
	{
		if (!hotBar.CanRemoveConsumableItem())
			return;
		hotBar.RemoveConsumableItem();

		VCU_LOG("1本");

		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom).Normalize() * FIRE_SPEED_PER_SEC);

		// 矢の発射音
		Get<AudioManager>().Play(shootArrowSE);
	}
}

/// @brief 更新処理
void ItemActionWeaponBow::Update(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput                = inputSystem.GetPlayerInput();

	if (pPlayerInput->IsAction())
	{
		if (!_playerHandData.isActing)
		{
			_playerHandData.isActing                         = true;
			_playerHandData.transformAnimator.playingTimeSec = 0.0f;
			_playerHandData.transformAnimator.playAnimation  = "bowstring";

			// 弓を引く音をループ再生
			Get<AudioManager>().PlayBowPull(pullBowSE);
		}

		_playerHandData.transformAnimator.playToLoop = true;
		stringTimeSec += Get<DeltaTime>();
	}
	else
	{
		// 弓を引く音を停止
		Get<AudioManager>().StopBowPull();

		if (_playerHandData.isActing)
		{
			_playerHandData.isActing                        = false;
			_playerHandData.transformAnimator.playToLoop    = false;
			_playerHandData.transformAnimator.playAnimation = "idle";

			VCU_LOG("アクション！");

			// 矢を発射
			Action(_itemId, _playerData, _playerHandData);

			stringTimeSec = 0.0f;
		}
	}

	if (_playerHandData.isActing)
	{
		if (_playerHandData.transformAnimator.isFinished)
		{
			_playerHandData.isActing = false;
		}
	}
	else
	{
		if (_playerHandData.transformAnimator.isFinished)
		{
			_playerHandData.transformAnimator.playAnimation  = "idle";
			_playerHandData.transformAnimator.playingTimeSec = 0.0f;
			_playerHandData.isActing                         = false;
		}
	}
}