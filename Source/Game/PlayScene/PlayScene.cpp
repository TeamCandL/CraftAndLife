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
#include "PlayerBag.h"
#include <../Resource/MAP/MAP.h>
#include "Harvestable.h"
#include "UI/MyGauge.h"
#include "NetworkActorServer.h"
#include "UI/MiniMap.h"
#include "UI/DeathUI.h"
#include "UI/CraftUI.h"
#include "HarvestableController.h"
#include <Game/Manager/AudioManager.h>
#include "ClearSceneTrigger.h"

void PlayScene::Start()
{
	// タイトルBGMを停止
	ManagerRegistry::Ref<AudioManager>().SetBGMStop();

	Instantiate<GameManager>();
	GameObject objSky = Instantiate<Sky>();

	GameObject objHarvestableController = Instantiate<HarvestableController>();
	GameObject objTerrain               = Instantiate<MAP>(objHarvestableController);
	// GameObject objTerrain = Instantiate<Terrain>();

	GameObject objDayController = Instantiate<DayController>(objSky);

	GameObject objMainCamera = Instantiate<MainCamera>();
	GameObject objPlayer     = Instantiate<Player>(objMainCamera);
	Instantiate<DeathUI>(objPlayer, true);

	// ホスト判定オブジェクトを生成
	GameObject objNetworkActorServer = Instantiate<NetworkActorServer>(objPlayer, objDayController);

	GameObject objOtherPlayerController = Instantiate<OtherPlayerController>();

	// debaug用に敵を生成する
	// GameObject objEnemyController = Instantiate<EnemyController>();
	// GameObject objSampleEnemy     = Instantiate<Enemy>(objEnemyController, EnemyType::Dragon);

	Instantiate<Reticle>();

	// ミニマップを生成してプレイヤーを渡す
	Instantiate<PlayNetworkController>(
	    objPlayer,
	    objMainCamera,
	    objNetworkActorServer,
	    objDayController,
	    objHarvestableController);

	GameObject objPlaySceneUIController = Instantiate<PlaySceneUIController>(objPlayer, objOtherPlayerController);
	// Instantiate<Harvestable>(HarvestableType::Tree, Vec3{ 0.0f, 0.0f, 0.0f }, 1.0f);

	// クリアシーン切り替え用
	Instantiate<ClearSceneTrigger>();
}
