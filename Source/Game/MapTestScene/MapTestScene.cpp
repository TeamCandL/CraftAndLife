#include "MapTestScene.h"
#include <..//Resource/MAP/MAP.h>
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>

void MapTestScene::Start()
{
	// シーンに必要なコアマネージャーおよび環境オブジェクトの生成
	Instantiate<GameManager>();
	Instantiate<DebugCamera>();
	Instantiate<Sky>();

	// 地形マップオブジェクト（MAP）の生成
	Instantiate<MAP>();
}