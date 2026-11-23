#include "LobbyScene.h"
#include <Game/Common/GameManager.h>
#include "LobbyPlayerRoleSelectButton.h"
#include "LobbyReadyButton.h"
#include "MemberList.h"
#include "LobbyNetworkController.h"
#include "LobbyUI.h"

void LobbyScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<LobbyReadyButton>();
	GameObject objRoleSelectButton = Instantiate<LobbyPlayerRoleSelectButton>();
	Instantiate<LobbyUI>(objRoleSelectButton);
	// GameObject objMemberList = Instantiate<MemberList>();
	Instantiate<LobbyNetworkController>();  //(objMemberList);
}
