#include "pch\pch.h"
#include "ItemActionShield.h"
#include "../PlayerHand.h"

void ItemActionShield::Update(
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
			_playerHandData.transformAnimator.playAnimation  = "holding";
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

void ItemActionShield::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
}
