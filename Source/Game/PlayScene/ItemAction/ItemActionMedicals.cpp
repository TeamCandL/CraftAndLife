#include "pch\pch.h"
#include "ItemActionMedicals.h"
#include "../PlayerHand.h"
#include "../Player.h"
#include "../MainCamera.h"
#include <Game/System/RaycastSystem.h>
#include "../OtherPlayer.h"
#include "../UI/HotBar.h"

void ItemActionMedicals::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	HotBar& hotBar = Get<HotBar>(_playerData.objHotbar);
	if (!hotBar.CanRemoveConsumableItem())
	{
		return;
	}

	Vec3 positionFrom = _playerData.transform.position + Vec3::Up() * 1.0f;
	Vec3 positionTo   = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();

	std::vector<RaycastHitInfo> hitsInfo =
	    Get<RaycastSystem>().RaycastAll(positionFrom, (positionTo - positionFrom).Normalize(), ACTION_RANGE);

	bool isHealing{ false };
	for (auto& hitInfo : hitsInfo)
	{
		if (hitInfo.pObject)
		{
			GameObject objHit = hitInfo.pObject->ToGameObject();
			Tag* pTag         = TryGet<Tag>(objHit);
			if (pTag && pTag->Has("OtherPlayer"))
			{
				isHealing = true;
				if (objHit == objHealTargetOtherPlayer && Get<OtherPlayer>(objHit).Data().hp <= 0)
				{
					healTimeLeftSec -= Get<DeltaTime>();
					if (healTimeLeftSec <= 0.0f)
					{
						healTimeLeftSec = HEAL_TIME;
						hotBar.RemoveConsumableItem();
						Get<OtherPlayer>(objHit).Data().hp = RESPAWN_HEALTH;
					}
				}
				else
				{
					if (Get<OtherPlayer>(objHit).Data().hp <= 0)
					{
						objHealTargetOtherPlayer = objHit;
						healTimeLeftSec          = HEAL_TIME;
					}
				}
			}
		}
	}

	if (!isHealing)
	{
		healTimeLeftSec = HEAL_TIME;
	}
}

void ItemActionMedicals::Update(
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
			_playerHandData.transformAnimator.playAnimation  = "usingMedicals";
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
		Action(_itemId, _playerData, _playerHandData);
		if (_playerHandData.transformAnimator.isFinished)
		{
			_playerHandData.isActing = false;
		}
	}
	else
	{
		if (_playerHandData.transformAnimator.isFinished)
		{
			_playerHandData.transformAnimator.playAnimation  = "idleMedicals";
			_playerHandData.transformAnimator.playingTimeSec = 0.0f;
			_playerHandData.isActing                         = false;
		}
	}
}
