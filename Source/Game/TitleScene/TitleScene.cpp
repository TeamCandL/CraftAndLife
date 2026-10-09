#include "pch\pch.h"
#include "TitleScene.h"
#include <Game/Common/GameManager.h>
#include "WordJoinButton.h"
#include "WordInputPanel.h"
#include "TitleNetworkController.h"

void TitleScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<TitleNetworkController>();

	GameObject objWordInputPanel = Instantiate<WordInputPanel>();
	Instantiate<WordJoinButton>(objWordInputPanel);

}
