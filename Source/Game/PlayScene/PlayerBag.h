#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include <limits>
static constexpr ItemId EMPTY_ITEM_ID{ std::numeric_limits<ItemId>::max() };

struct ItemStack
{
	ItemId id{ EMPTY_ITEM_ID };
	int count{ 0 };
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
	void ExpansionCapacity(const int _capacity);

	bool CanCraft(const ItemId _id);
	void Craft(const ItemId _id);
	void AddItem(const ItemId _id, const int _count);

  private:
	void Sort();
	size_t GetUseItemIdx(const CraftInfo& _craftInfo) const;
};