#include "pch/pch.h"
#include "MapTestScene.h"
#include <..  //Resource/MAP/MAP.h>
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>

void MapTestScene::Start()
{
	// クラスヘッダーが確実にインクルードされた状態で生成
	Instantiate<GameManager>();
	Instantiate<DebugCamera>();
	Instantiate<Sky>();

	Instantiate<MAP>();
}
