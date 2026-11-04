#include "pch\pch.h"
#include "TitleScene.h"
#include <Game/Common/GameManager.h>
#include "WordJoinButton.h"
#include "WordInputPanel.h"
#include "TitleNetworkController.h"
#include "TitleLogo.h"
#include "TitleLogoStartGuide.h"
#include "TitlePageContlroller.h"
#include "TitleBGM.h"

void TitleScene::Start()
{
	Instantiate<GameManager>();

	// タイトルBGM
	Instantiate<TitleBGM>();

	Instantiate<TitleNetworkController>();
	// Instantiate<TitleLogo>();
	// Instantiate<TitleLogoStartGuide>();
	Instantiate<TitlePageController>();

	GameObject objWordInputPanel = Instantiate<WordInputPanel>();
	Instantiate<WordJoinButton>(objWordInputPanel);
}