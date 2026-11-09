#include "LobbyScene.h"
#include <Game/Common/GameManager.h>
#include "LobbyReadyButton.h"
#include "MemberList.h"
#include "LobbyNetworkController.h"
#include "LobbyUI.h"
void LobbyScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<LobbyReadyButton>();
	Instantiate<LobbyUI>();
	// GameObject objMemberList = Instantiate<MemberList>();
	Instantiate<LobbyNetworkController>();  //(objMemberList);
}
