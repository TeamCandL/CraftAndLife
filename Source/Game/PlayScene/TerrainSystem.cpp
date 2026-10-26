#include "TerrainSystem.h"
#include "ITerrain.h"

void TerrainSystem::Start() {}

void TerrainSystem::Register(
    const GameObject _objTerrain)
{
	objMainTerrain = _objTerrain;
}

float TerrainSystem::GetHeight(
    const float _worldX,
    const float _worldZ)
{
	if (!objMainTerrain.IsEmpty())
	{
		return Get<ITerrain>(objMainTerrain).GetHeight(_worldX, _worldZ);
	}
	return 0.0f;
}
