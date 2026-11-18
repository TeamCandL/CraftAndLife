#include "pch\pch.h"
#include "ItemActionMedicals.h"
#include "../PlayerHand.h"
#include "../Player.h"
#include "../MainCamera.h"
#include <Game/System/RaycastSystem.h>
#include "../OtherPlayer.h"
#include "../UI/HotBar.h"
#include "../OtherPlayerController.h"
#include <Game/Manager/AudioManager.h>
#include <Game/Common/EffectObject.h>

void ItemActionMedicals::Start()
{
	objHealingEffect = GetScene().Instantiate<EffectObject>("Effect/Resurrecting.efkefc");
}

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
						Get<OtherPlayerController>(_playerData.objOtherPlayerController)
						    .AddRespawn(Get<OtherPlayer>(objHit).Data().name.content, RESPAWN_HEALTH);
						Get<EffectObject>(objHealingEffect).Stop();
					}
				}
				else
				{
					if (Get<OtherPlayer>(objHit).Data().hp <= 0)
					{
						//// 新しい蘇生対象を見つけたときだけ処理
						// if (objHealTargetOtherPlayer != objHit)
						//{
						//	objHealTargetOtherPlayer = objHit;
						//	healTimeLeftSec          = HEAL_TIME;

						//}
						// 蘇生開始SEを1回再生
						Get<AudioManager>().Play(resuscitationSE);
						objHealTargetOtherPlayer = objHit;
						healTimeLeftSec          = HEAL_TIME;
						// 治療エフェクトの再生開始
						EffectObject& effect = Get<EffectObject>(objHealingEffect);
						effect.Play();
						Vec3 pos = Get<Transform>(objHealTargetOtherPlayer).position;
						// エフェクトの座標を治療中のプレイヤーの座標にする
						effect.Data().transform.position = Get<Transform>(objHealTargetOtherPlayer).position;
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

			// 蘇生対象をリセット
			// objHealTargetOtherPlayer = {};

			// transformAnimator.playingTimeSec = 0.0f;
			// transformAnimator.playAnimation  = "idle";
			// 治療中のオブジェクトを空にする
			// healTimeLeftSec          = HEAL_TIME;
			objHealTargetOtherPlayer                     = GameObject::Empty();
			_playerHandData.transformAnimator.playToLoop = false;
			Get<EffectObject>(objHealingEffect).Stop();
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

void ItemActionMedicals::OnDestroy()
{
	Get<EffectObject>(objHealingEffect).Stop();
}
