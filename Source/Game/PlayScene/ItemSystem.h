#pragma once
#include <Game/Game.h>
#include "ItemId.h"

enum ItemType
{
	Weapons = 0,  ///<	武器
	Consumables,  ///<	消費アイテム
	Materials,    ///<	素材アイテム
	ItemTypeMax,  ///<	その他アイテム
};

struct CraftInfo
{
	const ItemId _id{};      ///<	クラフトに必要なアイテムID
	const int32_t _value{};  ///<	クラフトに必要なアイテムの個数
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
	int32_t _damage{};
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

	const float _coolTime{};  ///<	クールタイム
	const int32_t _damage{};  ///<	攻撃力
};

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
	Resource<Model>& GetModel(const ItemId _id);
};
