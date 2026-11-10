#include "pch\pch.h"
#include "ItemActionMedicals.h"
#include "../PlayerHand.h"
#include "../Player.h"
#include "../MainCamera.h"
#include <Game/System/RaycastSystem.h>

void ItemActionMedicals::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	Vec3 positionFrom = _playerData.transform.position + Vec3::Up() * 1.0f;
	Vec3 positionTo   = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();

	/*std::vector<RaycastHitInfo> hitsInfo =
	    Get<RaycastSystem>().RaycastAll(positionFrom, (positionTo - positionFrom).Normalize(), ACTION_RANGE);*/

	// for (auto& hitInfo : hitsInfo)
	//{
	//	if (hitInfo.pObject)
	//	{
	//		GameObject objHit = hitInfo.pObject->ToGameObject();
	//		Name* pName       = TryGet<Name>(objHit);
	//		if (pName)
	//		{
	//			VCU_LOG("{}に当たった！", pName->content);

	//			const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(_itemId);

	//			// ハーベスタブルオブジェクトならダメージを与える
	//			if (pName->content == "ハーベスタブル")
	//			{
	//
	//			}
	//		}
	//	}
	//}
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
			_playerHandData.transformAnimator.playAnimation  = "idleMedicals";
			_playerHandData.transformAnimator.playingTimeSec = 0.0f;
			_playerHandData.isActing                         = false;
		}
	}
}
