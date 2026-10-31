#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>

struct PlayerBag;

struct CraftUIData
{
  protected:
	ItemId selectedItemId{ INVALID_ITEM_ID };
};

struct CraftUI : ObjectBase<CraftUIData>
{
	using ObjectBase<CraftUIData>::ObjectBase;
	void Start() override;
	ItemId Update(PlayerBag& _playerBag);
};
