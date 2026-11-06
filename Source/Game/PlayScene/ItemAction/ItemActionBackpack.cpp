#include "pch\pch.h"
#include "ItemActionBackpack.h"
#include "../Player.h"
#include "../PlayerHand.h"

void ItemActionBackpack::Update(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	_playerHandData.transformAnimator.playAnimation  = "idle";
	_playerHandData.transformAnimator.playingTimeSec = 0.0f;
	_playerHandData.isActing                         = false;

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput                = inputSystem.GetPlayerInput();

	if (pPlayerInput->IsActionBegin())
	{
		_playerData.isOpenCraftUI = !_playerData.isOpenCraftUI;
	}
}

void ItemActionBackpack::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
}
