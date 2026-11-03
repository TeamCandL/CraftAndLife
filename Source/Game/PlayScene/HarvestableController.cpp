#include "pch\pch.h"
#include "HarvestableController.h"

void HarvestableController::AddHarvestable(
    HarvestableType _type,
    Vec3 _position,
    float _scale)
{
	uint32_t id = objs.size();
	objs.push_back(GetScene().Instantiate<Harvestable>(id, _type, _position, _scale));
}

std::optional<uint32_t> HarvestableController::FindToLocalBokenIds()
{
	for (auto& obj : objs)
	{
		Harvestable& harvestable = Get<Harvestable>(obj);
		if (harvestable.IsHarvested())
		{
			return harvestable.Data().id;
		}
	}
	return std::nullopt;
}

void HarvestableController::BreakAt(
    const uint32_t _id)
{
	Get<Harvestable>(objs[_id]).Break();
}
