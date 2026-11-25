#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include <Game/UI/ImageObject.h>
#include <Game/UI/TextObject.h>
#include <limits>

struct BagItemStack
{
	// バッグ内における、空のアイテム枠のID。バッグをソートする都合上、ItemId型の最大値に設定している。
	static constexpr ItemId EMPTY_STACK_ITEM_ID{ std::numeric_limits<ItemId>::max() };
	ItemId id{ EMPTY_STACK_ITEM_ID };
	int count{ 0 };
	// アイテムの最大スタック数
	int maxStack{ 64 };
	bool selected{ false };
	/// @brief 空か否か
	/// @return 空ならtrue
	bool IsEmpty() const
	{
		return count <= 0 || id == EMPTY_STACK_ITEM_ID;
	}
	void Reset();
	bool operator==(
	    const BagItemStack& _other) const
	{
		return id == _other.id && count == _other.count;
	}
	void CreateUI(SceneBase& _sceneBase);
	GameObject backgroundImage{};
	GameObject itemIconImage{};
	GameObject itemCountText{};
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
	std::vector<BagItemStack> items;

	// バッグの初期容量
	int initialCapacity{ 20 };

	// クラフト成功・失敗SE
	Resource<Audio> craftingSE{ "BGM_SE/Crafting.mp3" };
	Resource<Audio> mistakeSE{ "BGM_SE/Mistake.mp3" };

	Vec2 begin{ 425, 624 };
	Vec2 itemStackImageSize{ 90, 90 };
	// スタックの中心座標(パネルとの相対座標)
	Vec2 stackCenter{ 67, 64 };
	// スタックのアイテム数の座標(パネルとの相対座標)
	Vec2 leftSlotItemCountBegin{ 67, 58 };
	// 非選択時のスロット画像のサイズ
	int slotEmptyImageExtents{ 90 };
	// 選択時のスロット画像のサイズ
	int slotSelectedImageExtents{ 110 };
	// 1行に表示するアイテムのスタック数。それ以降は改行される
	int oneLineStackCount{ 10 };
	Resource<Texture> stackBackgroundTexture{ "Image/UI/slot_empty.png" };
	Resource<Texture> stackSelectedBackgroundTexture{ "Image/UI/slot_selected.png" };
	Resource<Texture> emptyItemIconTexture{ "Image/EmptyItem.png" };
};

struct PlayerBag : ObjectBase<PlayerBagData>
{
	PlayerBag();
	void Start() override;
	void Update() override;

	std::vector<BagItemStack> GetBagItems()
	{
		return items;
	}

	std::vector<ItemStack> GetItemStacks();

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

	/// @brief 指定したIdのアイテムを破棄する。破棄する個数は一つ
	/// @param _id
	/// @return 破棄に成功した場合true/失敗した場合false
	bool DiscardItem(const ItemId _id);
	void ClearItems();

  private:
	void UpdateUI();
	void Sort();
	/// @brief
	/// クラフトに必要な条件を満たしているアイテムのスロットを検索して、見つかればそのインデックスを返す。見つからなければ-1を返す
	/// @param _craftInfo クラフトに必要なアイテムの情報
	/// @return スロットへのインデックス。見つからなければ-1
	int FindCraftMaterialSlot(const CraftInfo& _craftInfo) const;
	void ResetItemStack(BagItemStack& _stack);
	void CreateStackUI(BagItemStack& _stack);
	void CalculateUIWorld(BagItemStack& _stack);
	void SetActiveAll(const bool _active);
};
