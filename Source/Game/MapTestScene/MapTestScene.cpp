#include "pch/pch.h"
#include "MapTestScene.h"

#include <../Resource/MAP/MAP.h>
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>
#include <Game/PlayScene/DebugPoint.h>
#include <Game/PlayScene/TerrainSystem.h>

void MapTestScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<DebugCamera>();
	Instantiate<Sky>();
	Instantiate<MAP>();
}