#include "ClearScene.h"
#include "ClearImage.h"
#include "ClearBGM.h"
#include <Game/Common/GameManager.h>
#include <Game/Manager/AudioManager.h>

void ClearScene::Start()
{
	// 環境音を停止
	ManagerRegistry::Ref<AudioManager>().StopAmbientSound();

	// ゲームマネージャーを生成
	Instantiate<GameManager>();

	// クリア画像を生成
	Instantiate<ClearImage>();

	// クリアBGMを再生
	Instantiate<ClearBGM>();
}