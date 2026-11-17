#include "pch\pch.h"
#include "ItemActionWeapons.h"
#include "../PlayerHand.h"
#include "../Player.h"
#include "../MainCamera.h"
#include <Game/System/RaycastSystem.h>
#include "../Harvestable.h"
#include <Game/Common/EffectObject.h>
#include "../Enemy.h"
#include "../EntitySystem.h"
#include <Game/Manager/AudioManager.h>

void ItemActionWeapons::Update(
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

void ItemActionWeapons::Action(
    const ItemId _itemId,
    PlayerData& _playerData,
    PlayerHandData& _playerHandData)
{
	const float ACTION_RANGE  // アイテムアクションの届く範囲
	    { _itemId == ITEM_ID_SWORD_BIG ? swordBigItemRange : standardItemRange };

	const ItemInfo ITEM_INFO{ Get<ItemSystem>().GetInfo(_itemId) };
	if (ITEM_INFO._type == ItemType::Weapons)
	{
		// 剣・大剣の攻撃音
		Get<AudioManager>().Play(_playerData.swingMissSE);

		Get<EntitySystem>().Spawn(
		    _itemId == ITEM_ID_SWORD_BIG ? ITEM_ID_PLAYER_ATTACK_SWORD_BIG : ITEM_ID_PLAYER_ATTACK_SWORD,
		    _playerData.transform.position + (_playerData.transform.Forward() * (ACTION_RANGE * 0.5f)),
		    _playerData.transform.Forward() * 20.0f);
		return;
	}

	// 斧・ハンマーの攻撃音
	if (_itemId == ITEM_ID_AXE || _itemId == ITEM_ID_HAMMER)
	{
		Get<AudioManager>().Play(_playerData.swingMissSE);
	}

	Vec3 positionFrom = _playerData.transform.position + Vec3::Up() * 1.0f;
	Vec3 positionTo   = Get<MainCamera>(_playerData.objMainCamera).GetFocusPosition();

	std::vector<RaycastHitInfo> hitsInfo =
	    Get<RaycastSystem>().RaycastAll(positionFrom, (positionTo - positionFrom).Normalize(), ACTION_RANGE);

	for (auto& hitInfo : hitsInfo)
	{
		if (hitInfo.pObject)
		{
			GameObject objHit = hitInfo.pObject->ToGameObject();
			Name* pName       = TryGet<Name>(objHit);
			if (pName)
			{
				VCU_LOG("{}に当たった！", pName->content);

				const ItemInfo ITEM_INFO = Get<ItemSystem>().GetInfo(_itemId);

				// ハーベスタブルオブジェクトならダメージを与える
				if (pName->content == "ハーベスタブル")
				{
					Harvestable& harvestable = Get<Harvestable>(objHit);

					switch (harvestable.Data().type)
					{
					case HarvestableType::Tree:
					case HarvestableType::TreeHigh:
						if (_itemId == ITEM_ID_AXE)
						{
							harvestable.TakeDamage(ITEM_INFO._damageForObject);
							GetScene().Instantiate<OnceEffectObject>("Effect/ScoreItem.efkefc", hitInfo.hitPosition);
						}
						break;
					case HarvestableType::Rock:
						if (_itemId == ITEM_ID_HAMMER)
						{
							harvestable.TakeDamage(ITEM_INFO._damageForObject);
							GetScene().Instantiate<OnceEffectObject>("Effect/ScoreItem.efkefc", hitInfo.hitPosition);
						}
						break;
					default:
						break;
					}
				}
				// プレイヤーの攻撃エンティティとして出します
				// else if (pName->content == "Enemy")
				//{
				//	// 当たっているのが敵なら敵へダメージを与える
				//	Enemy& enemy = Get<Enemy>(objHit);
				//	enemy.Damaged(ITEM_INFO._damageForEnemy);
				//}
			}
		}
	}
}
