#include "MapTestScene.h"
#include "pch\pch.h"
#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Terrain/Terrain.h>
// #include "自分で追加したおぶじぇくとのヘッダー"

void MapTestScene::Start()
{
	Instantiate<GameManager>();
	Instantiate<Sky>();
	// Instantiate<オッジェクトの型>();

	GameObject objTerrain = Instantiate<Terrain>();
}
