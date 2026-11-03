#include "pch\pch.h"
#include "ItemActionWeaponBow.h"
#include "../Player.h"
#include "../PlayerHand.h"
#include "../EntitySystem.h"
#include "../MainCamera.h"

void ItemActionWeaponBow::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	EntitySystem& entitySystem = Get<EntitySystem>();

	Vec3 firePositionFrom = _playerHandData.transform.WorldPosition();
	Vec3 firePositionTo   = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();
	if (stringTimeSec >= tripleFireTimeThresholdSec)
	{
		VCU_LOG("3本");
		Vec3 right = _playerData.transform.Right();

		// 真ん中
		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom).Normalize() * FIRE_SPEED_PER_SEC);

		// 左
		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom + right * -TRIPLE_FIRE_DISTANCEX).Normalize() * FIRE_SPEED_PER_SEC);

		// 右
		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom + right * TRIPLE_FIRE_DISTANCEX).Normalize() * FIRE_SPEED_PER_SEC);
	}
	else
	{
		VCU_LOG("1本");
		entitySystem.Spawn(
		    ARROW_ITEM_ID,
		    firePositionFrom,
		    (firePositionTo - firePositionFrom).Normalize() * FIRE_SPEED_PER_SEC);
	}
	stringTimeSec = 0.0f;
}

void ItemActionWeaponBow::Update(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput                = inputSystem.GetPlayerInput();

	if (pPlayerInput->IsAction())
	{
		if (!_playerHandData.isActing)
		{
			_playerHandData.isActing                         = true;
			_playerHandData.transformAnimator.playingTimeSec = 0.0f;
			_playerHandData.transformAnimator.playAnimation  = "bowstring";
		}

		_playerHandData.transformAnimator.playToLoop = true;  // 長押し中ならループ追加を要請

		stringTimeSec += Get<DeltaTime>();
	}
	else
	{
		if (_playerHandData.isActing)
		{
			_playerHandData.isActing                        = false;
			_playerHandData.transformAnimator.playToLoop    = false;
			_playerHandData.transformAnimator.playAnimation = "idle";

			VCU_LOG("アクション！");
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
