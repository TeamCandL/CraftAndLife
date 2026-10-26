#include "pch\pch.h"
#include "ItemSystem.h"

// ItemInfo ItemSystemData::GetInfo(int32_t id_)
//{
//	return {allItemList[id_]._imageId};
// }

ItemSystemData::ItemSystemData()
{
	std::vector<CraftInfo> craft;
	craft.push_back({ 0, 0 });
	allItemList.push_back({ 0,
	                        ItemType::Materials,
	                        "",
	                        "",
	                        "",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });

	allItemList.push_back({ 1,
	                        ItemType::Weapons,
	                        "さんぷる１",
	                        "",
	                        "Model/Sampe/Sword.fbx",

	                        true,
	                        craft,

	                        1.0f,
	                        10 });

	allItemList.push_back({ 1,
	                        ItemType::Weapons,
	                        "さんぷる２",
	                        "",
	                        "Model/Sampe/Axe.fbx",

	                        true,
	                        craft,

	                        2.0f,
	                        25 });
}

ItemInfo ItemSystem::GetInfo(
    const ItemId id)
{
	ItemInfo info;
	info._id   = allItemList[id]._id;
	info._type = allItemList[id]._type;
	info._name = allItemList[id]._name;

	info._isCraftable = allItemList[id]._isCraftable;
	info._craftInfo   = allItemList[id]._craftInfo;

	info._imageId  = allItemList[id]._imageId;
	info._coolTime = allItemList[id]._coolTime;
	info._damage   = allItemList[id]._damage;
	return info;
}
