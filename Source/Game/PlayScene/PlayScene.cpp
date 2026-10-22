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

void PlayScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<Sky>();

	GameObject objTerrain = Instantiate<Terrain>();

	Instantiate<OtherPlayer>("てすとん");

	Instantiate<OtherPlayerController>();
	Instantiate<ParticleSystem>();

	Instantiate<Enemy>(); // 仮設置
	Instantiate<Reticle>();
	GameObject objMainCamera = Instantiate<MainCamera>();
	GameObject objPlayer = Instantiate<Player>(objMainCamera);
	Instantiate<PlayNetworkController>(objPlayer, objMainCamera);
}
