#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include <limits>
static constexpr ItemId EMPTY_ITEM_ID{ std::numeric_limits<ItemId>::max() };

struct ItemStack
{
	ItemId id{ EMPTY_ITEM_ID };
	int count{ 0 };
	// アイテムの最大スタック数
	int maxStack{ 64 };

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
	bool operator==(const ItemStack& _other) const = default;
};

struct PlayerBagData
{
	PlayerBagData();

  protected:
	std::vector<ItemStack> items;

  private:
	/// @brief バッグの初期容量
	int initialCapacity{ 20 };
};

struct PlayerBag : ObjectBase<PlayerBagData>
{
	void Start() override;
	void Update() override;
	std::vector<ItemStack> GetBagItems()
	{
		return items;
	}
	/// @brief バッグの容量を拡張
	/// @param _capacity 拡張する容量
	void ExpansionCapacity(const unsigned int _capacity);

	/// @brief アイテムがクラフト可能かどうか判定
	/// @param _id クラフトしたいアイテムのID
	/// @return クラフト可能ならtrue
	bool CanCraft(const ItemId _id);
	/// @brief IDを指定してクラフト
	/// @param _id
	void Craft(const ItemId _id);
	/// @brief バッグにアイテムを追加する
	/// @param _id 追加するID
	/// @param _count 追加するアイテムの個数
	void AddItem(const ItemId _id, const unsigned int _count = 1);

  private:
	void Sort();
	/// @brief
	/// クラフトに必要な条件を満たしているアイテムのスロットを検索して、見つかればそのインデックスを返す。見つからなければ-1を返す
	/// @param _craftInfo クラフトに必要なアイテムの情報
	/// @return スロットへのインデックス。見つからなければ-1
	int FindCraftMaterialSlot(const CraftInfo& _craftInfo) const;
};