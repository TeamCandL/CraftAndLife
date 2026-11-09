#include "ClearScene.h"
#include "ClearImage.h"
#include "ClearBGM.h"
#include <Game/Common/GameManager.h>
#include <Game/Manager/AudioManager.h>

void ClearScene::Start()
{
	AudioManager& audioManager = ManagerRegistry::Ref<AudioManager>();

	// 環境音を停止
	audioManager.StopAmbientSound();

	// 足音を停止
	audioManager.StopFootstep();

	Instantiate<GameManager>();
	Instantiate<ClearImage>();
	Instantiate<ClearBGM>();
}