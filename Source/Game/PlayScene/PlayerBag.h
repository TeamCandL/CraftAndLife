#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include <limits>

struct ItemStack
{
	// バッグ内における、空のアイテム枠のID。バッグをソートする都合上、ItemId型の最大値に設定している。
	static constexpr ItemId EMPTY_STACK_ITEM_ID{ std::numeric_limits<ItemId>::max() };
	ItemId id{ EMPTY_STACK_ITEM_ID };
	int count{ 0 };
	// アイテムの最大スタック数
	int maxStack{ 64 };

	/// @brief 空か否か
	/// @return 空ならtrue
	bool IsEmpty() const
	{
		return count <= 0 || id == EMPTY_STACK_ITEM_ID;
	}
	static ItemStack EmptyStack()
	{
		return ItemStack{};
	}
	bool operator==(const ItemStack& _other) const = default;
};

// クラフト時の、素材の充足状況(足りているか、いくら入っているか)をまとめた構造体
struct CraftMaterialRequirement
{
	ItemId id;          // アイテムのID
	bool isSufficient;  // 個数が足りているか
	int ownedCount;     // バッグに入っている個数
	int requiredCount;  // クラフトに必要な個数
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
	/// @brief アイテムが追加可能か判定
	/// @param _id
	/// @return
	bool CanAddItem(const ItemId _id);
	/// @brief IDを指定してクラフト
	/// @param _id
	bool TryCraft(const ItemId _id);
	/// @brief バッグにアイテムを追加する
	/// @param _id 追加するID
	/// @param _count 追加するアイテムの個数
	void AddItem(const ItemId _id);
	/// @brief 指定したアイテムのクラフトに必要な素材の充足状況の配列を返す
	/// @param _craftItemId
	/// @return ItemInfoの vector<CraftInfo> _craftInfoと同じ順番で格納された、素材の充足状況
	std::vector<CraftMaterialRequirement> GetCraftRequirements(const ItemId _craftItemId);

  private:
	void Sort();
	/// @brief
	/// クラフトに必要な条件を満たしているアイテムのスロットを検索して、見つかればそのインデックスを返す。見つからなければ-1を返す
	/// @param _craftInfo クラフトに必要なアイテムの情報
	/// @return スロットへのインデックス。見つからなければ-1
	int FindCraftMaterialSlot(const CraftInfo& _craftInfo) const;
};