#include "pch\pch.h"
#include "ItemActionConsumables.h"
#include "../PlayerHand.h"

void ItemActionConsumables::Update(
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
			_playerHandData.transformAnimator.playAnimation  = "action";
		}

		_playerHandData.transformAnimator.playToLoop = true;  // 長押し中ならループ追加を要請
	}
	else
	{
		if (_playerHandData.isActing)
		{
			_playerHandData.isActing = false;
			// transformAnimator.playingTimeSec = 0.0f;
			// transformAnimator.playAnimation  = "idle";
			_playerHandData.transformAnimator.playToLoop = false;
		}
	}

	if (_playerHandData.isActing)
	{
		if (_playerHandData.transformAnimator.onFinished)
		{
			Action(_itemId, _playerData, _playerHandData);
		}

		if (_playerHandData.transformAnimator.isFinished)
		{
			_playerHandData.isActing = false;
			Action(_itemId, _playerData, _playerHandData);
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

void ItemActionConsumables::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
}
