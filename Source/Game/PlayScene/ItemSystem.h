#pragma once
#include <Game/Game.h>
#include "ItemId.h"
#include "Player/PlayerJobType.h"
#include "PlayerRoleType.h"

enum ItemType
{
	Weapons = 0,        ///<	武器
	Tools,              ///<	道具
	WeaponBow,          ///<	弓
	Shield,             ///<	盾
	Arrow,              ///<	矢
	Consumables,        ///<	消費アイテム
	Materials,          ///<	素材アイテム
	Medicals,           ///<	蘇生アイテム
	Backpack,           ///<	バックパック
	EnemyAttack,        ///<	敵の攻撃アイテム
	PlayerAttackSword,  ///<	プレイヤーの剣攻撃アイテム
	PlayerAttackArrow,  ///<	プレイヤーの弓攻撃アイテム
	ItemTypeMax,        ///<	その他アイテム
};

struct CraftInfo
{
	ItemId _id{};      ///<	クラフトに必要なアイテムID
	int32_t _value{};  ///<	クラフトに必要なアイテムの個数
};
struct ItemInfo
{
	ItemId _id{};
	ItemType _type{};
	std::string _name{};
	Resource<Texture> _imageId{};

	bool _isCraftable{};
	std::vector<CraftInfo> _craftInfo{};

	float _coolTime{};
	int32_t _damageForEnemy{};
	int32_t _damageForObject{};
	int32_t _damageForMob{};

	PlayerJobType canUseJobType{};

	Vec3 colliderSize{};  ///< コライダーの大きさ
};

struct ItemAllInfo
{
	const ItemId _id{};                  ///<	アイテムID
	const ItemType _type{};              ///<	アイテムの種類
	const std::string _name{};           ///<	アイテム名
	const Resource<Texture> _imageId{};  ///<	アイテム画像ID
	const Resource<Model> _model{};      ///<	アイテムモデル

	const bool _isCraftable{};                  ///<	クラフト可能か
	const std::vector<CraftInfo> _craftInfo{};  ///<	クラフトに必要なアイテムの情報

	const float _coolTime{};           ///<	クールタイム
	const int32_t _damageForEnemy{};   ///<	敵への攻撃力
	const int32_t _damageForObject{};  ///<	オブジェクトへの攻撃力
	const int32_t _damageForMob{};     ///<	モブへの攻撃力

	PlayerJobType canUseJobType{ PlayerJobType_Max };  ///<	使用可能な職業タイプ

	Vec3 colliderSize{};  ///< コライダーの大きさ
};

struct ItemStack
{
	ItemId id{ EMPTY_ITEM_ID };
	int count{ 0 };

	/// @brief 空か否か
	/// @return 空ならtrue
	bool IsEmpty() const
	{
		return count <= 0 || id == EMPTY_ITEM_ID;
	}
	static ItemStack EmptyStack()
	{
		return ItemStack{};
	}
};

bool JobCanCraftOrAddItem(const ItemType _type, PlayerJobType _job);

struct ItemSystemData
{
	ItemSystemData();
	/// @brief すべてのアイテムの全情報を格納する
	std::vector<ItemAllInfo> allItemList{};
};

struct ItemSystem : SystemObjectBase<ItemSystem, ItemSystemData>
{
	/// @brief ItemInfo格納用の情報を取得する
	/// @param id_ 情報を取得したいアイテムのID
	/// @return
	ItemInfo GetInfo(const ItemId id);

	/// @brief
	/// @param _id
	/// @return
	Resource<Model> GetModel(const ItemId _id);

	/// @brief 攻撃用アイテムIdをプレイヤーが取得可能なアイテムIdへ変換する
	/// @param _attackItemId 攻撃用ItemId
	/// @return アタック用アイテムId
	ItemId ConvertAttackItemToCollectibleItem(const ItemId _attackItemId);
};
