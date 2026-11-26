#include "pch\pch.h"
#include "TestPlayer.h"
#include <Game/PlayScene/Gift/GiftRequestSelector.h>
#include <Game/System/AbstructInputSystem.h>

void TestPlayer::Update()
{
	if (Get<AbstructInputSystem>().GetPlayerInput()->IsMessageBegin())
	{
		if (objGiftRequestSelector.IsEmpty())
		{
			objGiftRequestSelector = GetScene().Instantiate<GiftRequestSelector>(std::vector<ItemId>{
			    7,
			    8,
			    10,
			    14,
			});
		}
		else
		{
			std::optional<ItemId> selectedItemId =
			    Get<GiftRequestSelector>(objGiftRequestSelector).TryGetSelectedItemId();
			if (selectedItemId.has_value())  // 選択されているアイテムIdがある！
			{
				VCU_LOG("選択されているアイテムIdは{}", *selectedItemId);
			}

			GetScene().Destroy(objGiftRequestSelector);
		}
	}
}
