#include "PlayScene.h"
#include "Enemy.h"
#include "EnemyController.h"
#include "MainCamera.h"
#include "OtherPlayer.h"
#include "OtherPlayerController.h"
#include "PlayNetworkController.h"
#include "Player.h"
#include "UI/Reticle.h"
#include "pch\pch.h"
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Terrain/Terrain.h>
#include "DayController.h"
#include "OtherEnemy.h"
#include "OtherEnemyController.h"
#include "PlaySceneUIController.h"
#include <../Resource/MAP/MAP.h>
#include "Harvestable.h"
#include "UI/MyGauge.h"
#include "NetworkActorServer.h"

void PlayScene::Start()
{
	Instantiate<GameManager>();
	GameObject objSky = Instantiate<Sky>();

	GameObject objTerrain = Instantiate<MAP>();
	// GameObject objTerrain = Instantiate<Terrain>();

	GameObject objDayController = Instantiate<DayController>(objSky);

	// ホスト判定オブジェクトを生成
	GameObject objNetworkActorServer = Instantiate<NetworkActorServer>();

	Instantiate<OtherPlayerController>();
	// Instantiate<OtherEnemyController>();

	Instantiate<Reticle>();
	GameObject objMainCamera = Instantiate<MainCamera>();
	GameObject objPlayer     = Instantiate<Player>(objMainCamera);
	Instantiate<PlayNetworkController>(objPlayer, objMainCamera, objNetworkActorServer, objDayController);

	GameObject objPlaySceneUIController = Instantiate<PlaySceneUIController>(objPlayer);

	Instantiate<Harvestable>(HarvestableType::Tree, Vec3{ 0.0f, 0.0f, 0.0f }, 1.0f);
}
