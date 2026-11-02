#include "pch\pch.h"
#include "ItemSystem.h"

ItemSystemData::ItemSystemData()
{
	std::vector<CraftInfo> craft;
	craft.push_back({ 0, 0 });
	allItemList.push_back({ 0,
	                        ItemType::Materials,
	                        "",
	                        "Image/EmptyItem.png",
	                        {},

	                        false,
	                        craft,

	                        0.0f,
	                        0 });

	allItemList.push_back({ 1,
	                        ItemType::Weapons,
	                        "さんぷる１",
	                        "Image/InvalidImage.png",
	                        "Model/HandMaterial/HandWood.fbx",
	                        //"Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        1.0f,
	                        10 });

	allItemList.push_back({ 2,
	                        ItemType::Weapons,
	                        "さんぷる２",
	                        "Image/InvalidImage.png",
	                        "Model/Sample/Axe.fbx",
	                        //"Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        2.0f,
	                        25 });
	craft.clear();
	craft.push_back({ 8, 3 });
	craft.push_back({ 10, 2 });
	allItemList.push_back({ 3,
	                        ItemType::WeaponBow,
	                        "弓",
	                        "Image/Item/IconBow.png",
	                        "Model/HandWeapon/HandBow.fbx",

	                        true,
	                        craft,

	                        0.5f,
	                        8 });
	craft.clear();
	craft.push_back({ 7, 3 });
	craft.push_back({ 8, 2 });
	allItemList.push_back({ 4,
	                        ItemType::Weapons,
	                        "剣",
	                        "Image/Item/IconSword.png",
	                        "Model/HandWeapon/HandSword.fbx",

	                        true,
	                        craft,

	                        0.6f,
	                        12 });
	craft.clear();
	craft.push_back({ 7, 5 });
	craft.push_back({ 8, 3 });
	allItemList.push_back({ 5,
	                        ItemType::Weapons,
	                        "ハンマー",
	                        "Image/InvalidImage.png",
	                        "Model/HandWeapon/HandAxe.fbx",

	                        true,
	                        craft,

	                        1.0f,
	                        20 });
	craft.clear();
	craft.push_back({ 7, 3 });
	craft.push_back({ 8, 3 });
	allItemList.push_back({ 6,
	                        ItemType::Weapons,
	                        "斧",
	                        "Image/Item/IconAxe.png",
	                        "Model/HandWeapon/HandAxe.fbx",

	                        true,
	                        craft,

	                        0.8f,
	                        10 });
	allItemList.push_back({ 7,
	                        ItemType::Materials,
	                        "石",
	                        "Image/InvalidImage.png",
	                        "Model/HandMaterial/HandRock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 8,
	                        ItemType::Materials,
	                        "木材",
	                        "Image/InvalidImage.png",
	                        "Model/HandMaterial/HandWood.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 9,
	                        ItemType::Materials,
	                        "骨",
	                        "Image/InvalidImage.png",
	                        "Model/HandMaterial/HandRock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 10,
	                        ItemType::Materials,
	                        "蜘蛛の糸",
	                        "Image/InvalidImage.png",
	                        "Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 11,
	                        ItemType::Materials,
	                        "生肉",
	                        "Image/InvalidImage.png",
	                        "Model/HandFood/HandMeat.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 12,
	                        ItemType::Materials,
	                        "爪",
	                        "Image/InvalidImage.png",
	                        "Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 13,
	                        ItemType::Consumables,
	                        "焼肉",
	                        "Image/InvalidImage.png",
	                        "Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	allItemList.push_back({ 14,
	                        ItemType::Consumables,
	                        "キノコ",
	                        "Image/Item/IconMushroom.png",
	                        "Model/HandFood/HandMushroom.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0 });
	craft.clear();
	craft.push_back({ 7, 1 });
	craft.push_back({ 8, 2 });
	allItemList.push_back({ 15,
	                        ItemType::Arrow,
	                        "矢",
	                        "Image/Item/IconArrow.png",
	                        "Model/HandWeapon/Arrow.fbx",

	                        true,
	                        craft,

	                        0.0f,
	                        0 });
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

Resource<Model> ItemSystem::GetModel(
    const ItemId _id)
{
	return allItemList[_id]._model;
}