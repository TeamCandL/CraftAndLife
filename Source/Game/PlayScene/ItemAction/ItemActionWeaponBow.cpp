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

	Vec3 firePositionFrom = _playerHandData.transform.position;
	Vec3 firePositionTo   = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();
	if (stringTimeSec >= tripleFireTimeThresholdSec)
	{
		entitySystem.Spawn(ARROW_ITEM_ID, , )
	}
	else
	{
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
