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

Vec3 TerrainSystem::ToSafePosition(
    const Vec3 _position,
    const Vec3 _marginWidth)
{
	ITerrain& terrain = Get<ITerrain>(objMainTerrain);

	Vec3 posMin = terrain.GetMinPosition() - _marginWidth;
	Vec3 posMax = terrain.GetMaxPosition() + _marginWidth;

	return Vec3{
		std::clamp(_position.x, posMin.x, posMax.x),
		std::clamp(_position.y, posMin.y, posMax.y),
		std::clamp(_position.z, posMin.z, posMax.z),
	};
}
