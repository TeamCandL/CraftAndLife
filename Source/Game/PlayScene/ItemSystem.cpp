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
	                        0,
	                        0,
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
	                        10,
	                        0,
	                        0 });

	allItemList.push_back({ 2,
	                        ItemType::Weapons,
	                        "さんぷる２",
	                        "Image/InvalidImage.png",
	                        "Model/Sample/Axe.fbx",
	                        //"Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        2.0f,
	                        0,
	                        25,
	                        0,

	                        PlayerJobType_Max });
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
	                        10,
	                        0,
	                        0,

	                        PlayerJobType_Archer });
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
	                        15,
	                        0,
	                        0,

	                        PlayerJobType_Max });
	craft.clear();
	craft.push_back({ 7, 5 });
	craft.push_back({ 8, 3 });
	allItemList.push_back({ 5,
	                        ItemType::Tools,
	                        "ハンマー",
	                        "Image/Item/IconHammer.png",
	                        "Model/HandWeapon/HandHammer.fbx",

	                        true,
	                        craft,

	                        1.0f,
	                        0,
	                        5,
	                        0,

	                        PlayerJobType_Max });
	craft.clear();
	craft.push_back({ 7, 3 });
	craft.push_back({ 8, 3 });
	allItemList.push_back({ 6,
	                        ItemType::Tools,
	                        "斧",
	                        "Image/Item/IconAxe.png",
	                        "Model/HandWeapon/HandAxe.fbx",

	                        true,
	                        craft,

	                        0.8f,
	                        0,
	                        5,
	                        0 });
	allItemList.push_back({ 7,
	                        ItemType::Materials,
	                        "石",
	                        "Image/Item/IconRock.png",
	                        "Model/HandMaterial/HandRock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 8,
	                        ItemType::Materials,
	                        "木材",
	                        "Image/Item/IconWood.png",
	                        "Model/HandMaterial/HandWood.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 9,
	                        ItemType::Materials,
	                        "骨",
	                        "Image/Item/IconBone.png",
	                        "Model/HandMaterial/HandBone.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0 });
	allItemList.push_back({ 10,
	                        ItemType::Materials,
	                        "蜘蛛の糸",
	                        "Image/Item/IconString.png",
	                        "Model/HandMaterial/HandString.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 11,
	                        ItemType::Materials,
	                        "生肉",
	                        "Image/Item/IconMeat.png",
	                        "Model/HandFood/HandMeat.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 12,
	                        ItemType::Materials,
	                        "爪",
	                        "Image/Item/IconClaw.png",
	                        "Model/HandMaterial/HandClaw.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 13,
	                        ItemType::Consumables,
	                        "焼肉",
	                        "Image/InvalidImage.png",
	                        "Model/Harvestable/Rock.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 14,
	                        ItemType::Materials,
	                        "キノコ",
	                        "Image/Item/IconMushroom.png",
	                        "Model/HandFood/HandMushroom.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0 });
	craft.clear();
	craft.push_back({ 7, 1 });
	craft.push_back({ 8, 1 });
	allItemList.push_back({ 15,
	                        ItemType::Arrow,
	                        "矢",
	                        "Image/Item/IconArrow.png",
	                        "Model/HandWeapon/Arrow.fbx",

	                        true,
	                        craft,

	                        0.0f,
	                        10,  // TODO: 仮
	                        0,
	                        0,

	                        PlayerJobType_Archer,
	                        Vec3{
	                            1,
	                            1,
	                            1,
	                        } });
	craft.clear();
	craft.push_back({ 7, 3 });
	craft.push_back({ 8, 10 });
	allItemList.push_back({ 16,
	                        ItemType::Shield,
	                        "盾",
	                        "Image/Item/IconShield.png",
	                        "Model/HandWeapon/HandShield.fbx",

	                        true,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0,

	                        PlayerJobType_Tank });
	craft.clear();
	craft.push_back({ 7, 7 });
	craft.push_back({ 8, 4 });
	allItemList.push_back({ 17,
	                        ItemType::Weapons,
	                        "大剣",
	                        "Image/Item/IconSwordBig.png",
	                        "Model/HandWeapon/HandSwordBig.fbx",

	                        true,
	                        craft,

	                        2.0f,
	                        35,
	                        10,
	                        0,

	                        PlayerJobType_Fighter });
	craft.clear();
	craft.push_back({ 10, 5 });
	craft.push_back({ 14, 2 });
	allItemList.push_back({ 18,
	                        ItemType::Medicals,
	                        "医療キット",
	                        "Image/Item/IconMedicalKit.png",
	                        "Model/HandWeapon/HandMedicalKit.fbx",

	                        true,
	                        craft,

	                        5.0f,
	                        0,
	                        0,
	                        0,

	                        PlayerJobType_Healer });
	craft.clear();
	allItemList.push_back({ 19,
	                        ItemType::Backpack,
	                        "バックパック",
	                        "Image/Item/IconBackPack.png",
	                        "Model/HandBackPack/HandBackPack.fbx",

	                        false,
	                        craft,

	                        0.0f,
	                        0,
	                        0,
	                        0 });
	allItemList.push_back({ 20,
	                        ItemType::EnemyAttack,
	                        "糸攻撃",
	                        "Image/EmptyItem.png",
	                        "Model/HandMaterial/HandString.fbx",

	                        false,
	                        craft,

	                        10.0f,
	                        5,
	                        0,
	                        5,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
	allItemList.push_back({ 21,
	                        ItemType::EnemyAttack,
	                        "近距離攻撃",
	                        {},
	                        {},
	                        false,
	                        craft,
	                        1.0f,
	                        15,
	                        0,
	                        15,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
	allItemList.push_back({ 22,
	                        ItemType::PlayerAttackSword,
	                        "プレイヤー剣の攻撃",
	                        {},
	                        {},
	                        false,
	                        craft,
	                        1.0f,
	                        5,
	                        0,
	                        5,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 2 } });
	allItemList.push_back({ 23,
	                        ItemType::PlayerAttackSword,
	                        "プレイヤー大剣の攻撃",
	                        {},
	                        {},
	                        false,
	                        craft,
	                        1.0f,
	                        5,
	                        0,
	                        5,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 5 } });
	allItemList.push_back({ 24,
	                        ItemType::PlayerAttackArrow,
	                        "プレイヤー矢の攻撃",
	                        {},
	                        "Model/HandWeapon/Arrow.fbx",
	                        false,
	                        craft,
	                        1.0f,
	                        5,
	                        0,
	                        5,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 5 } });
	allItemList.push_back({ 25,
	                        ItemType::Materials,
	                        "じゅえる",
	                        {},
	                        {},
	                        false,
	                        craft,
	                        0.0f,
	                        0,
	                        0,
	                        0,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
	allItemList.push_back({ 26,
	                        ItemType::EnemyAttack,
	                        "火ィ攻撃",
	                        {},
	                        {},

	                        false,
	                        craft,

	                        12.0f,
	                        12,
	                        0,
	                        5,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
	allItemList.push_back({ 27,
	                        ItemType::PlayerAttackTool,
	                        "ハンマー攻撃",
	                        "Image/EmptyItem.png",
	                        {},

	                        false,
	                        craft,

	                        1.0f,
	                        2,
	                        0,
	                        0,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
	allItemList.push_back({ 28,
	                        ItemType::PlayerAttackTool,
	                        "斧攻撃",
	                        "Image/EmptyItem.png",
	                        {},

	                        false,
	                        craft,

	                        1.0f,
	                        2,
	                        0,
	                        0,
	                        PlayerJobType_Max,
	                        Vec3{ 1, 1, 1 } });
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

	info._imageId         = allItemList[id]._imageId;
	info._coolTime        = allItemList[id]._damageLifeTime;
	info._damageForEnemy  = allItemList[id]._damageForEnemy;
	info._damageForObject = allItemList[id]._damageForObject;

	info.canUseJobType = allItemList[id].canUseJobType;
	info.colliderSize  = allItemList[id].colliderSize;
	return info;
}

Resource<Model> ItemSystem::GetModel(
    const ItemId _id)
{
	return allItemList[_id]._model;
}

ItemId ItemSystem::ConvertAttackItemToCollectibleItem(
    const ItemId _attackItemId)
{
	switch (_attackItemId)
	{
	case 20:  // 蜘蛛の糸
		return 10;
	case 24:  // 矢
		return 15;
	default:  // 他しらん！
		VCU_ASSERT(false && "指定のアイテムIdは変換できない！");
		return 0;
	}
}

bool JobCanCraftOrAddItem(
    const ItemType _type,
    PlayerJobType _job)
{
	if (_type == Arrow || _type == WeaponBow)
	{
		return _job == PlayerJobType_Archer;
	}
	return true;
}
