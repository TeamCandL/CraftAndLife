#pragma once
#include <Game/Game.h>

struct CraftInfo
{
	const int32_t id{}; ///<	クラフトに必要なアイテムID
	const int32_t value{}; ///<	クラフトに必要なアイテムの個数
};

struct ItemAllInfo
{
	const int32_t _id{}; ///<	アイテムID
	const ItemType _type{}; ///<	アイテムの種類
	const std::string _name{}; ///<	アイテム名
	const int _imageId{};      ///<	アイテム画像ID
	const Resource<Model> _model{}; ///<	アイテム画像

	const bool _isCraftable{};      ///<	クラフト可能か
	const std::vector<CraftInfo> _craftInfo{}; ///<	クラフトに必要なアイテムの情報

	const float _coolTime{};                   ///<	クールタイム
	const int32_t _damage{};                   ///<	攻撃力
};
enum ItemType
{
	Weapons = 0, ///<	武器
	Consumables, ///<	消費アイテム
	Materials,   ///<	素材アイテム
	ItemTypeMax,      ///<	その他アイテム
};

struct ItemInfo;

struct ItemSystemData
{
	/// @brief すべてのアイテムの情報を格納する
	std::vector<ItemAllInfo> allItemList{};
	
	ItemInfo GetInfo(int32_t id_);
};