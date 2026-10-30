#include "pch\pch.h"
#include "TitleScene.h"
#include <Game/Common/GameManager.h>
#include "WordJoinButton.h"
#include "WordInputPanel.h"
#include "TitleNetworkController.h"
#include "TitleLogo.h"

void TitleScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<TitleNetworkController>();
	Instantiate<TitleLogo>();

	GameObject objWordInputPanel = Instantiate<WordInputPanel>();
	Instantiate<WordJoinButton>(objWordInputPanel);
}
