#include "LobbyScene.h"
#include <Game/Common/GameManager.h>
#include "LobbyReadyButton.h"
#include "MemberList.h"
#include "LobbyNetworkController.h"

void LobbyScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<LobbyReadyButton>();
	GameObject objMemberList = Instantiate<MemberList>();

	Instantiate<LobbyNetworkController>(objMemberList);
}
