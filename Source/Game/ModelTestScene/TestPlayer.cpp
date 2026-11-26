#include "pch\pch.h"
#include "TestPlayer.h"
#include <Game/PlayScene/Gift/GiftRequestBox.h>
#include <Game/System/AbstructInputSystem.h>

void TestPlayer::Update()
{
	if (Get<AbstructInputSystem>().GetPlayerInput()->IsMessageBegin())
	{
		if (objGiftRequestBox.IsEmpty())
		{
			objGiftRequestBox = GetScene().Instantiate<GiftRequestBox>();
		}
		else
		{
			GetScene().Destroy(objGiftRequestBox);
		}
	}
}
