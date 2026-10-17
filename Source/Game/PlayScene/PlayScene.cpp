#include "pch\pch.h"
#include "PlayScene.h"
#include <Game/Common/GameManager.h>
#include "PlayNetworkController.h"
#include <Game/Common/Sky.h>
#include "UI/Reticle.h"
#include "MainCamera.h"
#include "Player.h"
#include <Game/Terrain/Terrain.h>

void PlayScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<Sky>();

	GameObject objTerrain = Instantiate<Terrain>();

	Instantiate<Reticle>();
	GameObject objMainCamera = Instantiate<MainCamera>();
	GameObject objPlayer = Instantiate<Player>(objMainCamera);
	Instantiate<PlayNetworkController>(objPlayer);
}
