#include "pch\pch.h"
#include "TerrainController.h"
#include <Game/Terrain/Terrain.h>

void TerrainController::Start()
{
	objMainTerrain = GetScene().Instantiate<Terrain>();
}

void TerrainController::Update()
{

}

void TerrainController::Draw()
{
}
