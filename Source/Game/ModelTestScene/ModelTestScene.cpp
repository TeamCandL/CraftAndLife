#include "pch\pch.h"
#include "ModelTestScene.h"
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <../Resource/MAP/MAP.h>
#include <Game/Tool/DebugCamera.h>
#include <Game/PlayScene/HarvestableController.h>
#include <Game/PlayScene/NPC/NPCRobo.h>
#include <Game/PlayScene/Gift/GiftRequestBox.h>
#include "TestPlayer.h"

void ModelTestScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<DebugCamera>();
	Instantiate<Sky>();
	Instantiate<MAP>(Instantiate<HarvestableController>());
	Instantiate<NPCRobo>();
	Instantiate<TestPlayer>();
}
