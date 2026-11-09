#include "BootScene.h"
#include "BootSceneBackground.h"
#include "BootSceneLogo.h"
#include <Game/Common/GameManager.h>
#include <Game/Manager/AudioManager.h>

void BootScene::Start()
{
	Instantiate<GameManager>();

	Instantiate<BootSceneLogo>();
	Instantiate<BootSceneBackground>();

	// Startup.mp3 → ChaEngine.mp3 の順番で再生
	ManagerRegistry::Ref<AudioManager>().PlaySequence(startupSE, chaEngineSE);
}