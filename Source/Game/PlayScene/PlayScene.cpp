#include "PlayScene.h"
#include "Enemy.h"
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

void PlayScene::Start()
{
	Instantiate<GameManager>();
	GameObject objSky = Instantiate<Sky>();

	GameObject objTerrain = Instantiate<Terrain>();

	Instantiate<OtherPlayer>("てすとん");

	Instantiate<DayController>(objSky);
	Instantiate<Enemy>();  // 仮設置
	Instantiate<OtherEnemy>(1, EnemyType::Skeleton);
	Instantiate<OtherPlayerController>();
	Instantiate<OtherEnemyController>();

	Instantiate<Reticle>();
	GameObject objMainCamera = Instantiate<MainCamera>();
	GameObject objPlayer     = Instantiate<Player>(objMainCamera);
	GameObject objEnemy      = Instantiate<Enemy>();
	Instantiate<PlayNetworkController>(objPlayer, objMainCamera, objEnemy);

	GameObject objPlaySceneUIController = Instantiate<PlaySceneUIController>();
}
