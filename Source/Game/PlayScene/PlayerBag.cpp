#include "PlayerBag.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Manager/AudioManager.h>

#include <numeric>

PlayerBagData::PlayerBagData()
{
	items.resize(initialCapacity, BagItemStack::EmptyStack());
}

void PlayerBag::Start() {}

void PlayerBag::Update()
{
	static ItemId id{ 0 };
	auto& status = Get<SessionManager>().RefStatus();
#ifdef USE_IMGUI
	// デバッグ用のUI
	ImGui::Begin(std::format("Bag:{}", status.name.c_str()).c_str());
	if (ImGui::Button("ExpansiionCapa"))
	{
		ExpansionCapacity(10);
	}

	// アイテム追加用のUI(デバッグ)
	ImGui::InputInt("ID", &id);
	if (ImGui::Button("Add Item"))
	{
		AddItem(id);
	}

	for (int i = 0; i < items.size(); i++)
	{
		if (items[i].id == BagItemStack::EMPTY_STACK_ITEM_ID)
		{
			ImGui::Text("%d:empty, %d", i, items[i].count);
		}
		else
		{
			std::string itemName = Get<ItemSystem>().GetInfo(items[i].id)._name;
			ImGui::Text("%d:%s %d, %d", i, itemName.c_str(), items[i].id, items[i].count);
		}
	}
	ImGui::End();
#endif
}

std::vector<ItemStack> PlayerBag::GetItemStacks()
{
	std::vector<ItemStack> itemStacks;
	for (auto& item : items)
	{
		if (item.IsEmpty() == false)
		{
			ItemId id = item.id;
			auto itr  = std::find_if(
                itemStacks.begin(),
                itemStacks.end(),
                [id](const ItemStack& _stack)
                {
                    return _stack.id == id;
                });

			// 既に同じアイテムが入っているなら加算
			if (itr != itemStacks.end())
			{
				itr->count += item.count;
			}
			// 無いなら新たに追加
			else
			{
				itemStacks.emplace_back(id, item.count);
			}
		}
	}
	return itemStacks;
}

void PlayerBag::ExpansionCapacity(
    const unsigned _capacity)
{
	// バッグの容量を拡張。新規スロットは空の状態で追加する
	items.resize(items.size() + _capacity, BagItemStack::EmptyStack());
}

bool PlayerBag::CanCraft(
    const ItemId _id)
{
	ItemInfo itemInfo      = Get<ItemSystem>().GetInfo(_id);
	const auto& craftInfos = itemInfo._craftInfo;

	for (const CraftInfo& craftInfo : craftInfos)
	{
		int idx = FindCraftMaterialSlot(craftInfo);
		if (idx == -1)
			return false;
	}
	return true;
}

bool PlayerBag::CanAddItem(
    const ItemId _id)
{
	ItemType itemType = Get<ItemSystem>().GetInfo(_id)._type;
	if (itemType == ItemType::Weapons)
		return false;

	// 同じIDのアイテムの範囲を取得
	auto [begin, end] = std::equal_range(
	    items.begin(),
	    items.end(),
	    BagItemStack{ .id = _id },
	    [](const BagItemStack& _a, const BagItemStack& _b)
	    {
		    return _a.id < _b.id;
	    });

	// 同じIDのアイテムのスロットを走査
	for (auto itr = begin; itr != end; itr++)
	{
		const int max          = itr->maxStack;
		const int currentCount = itr->count;
		if (itr->maxStack == itr->count)
		{
			continue;
		}

		// スロットのスタック数が最大スタック数を超えない
		if (currentCount < max)
		{
			return true;
		}
	}

	// 空スロットを検索
	auto emptySlot = std::find(items.begin(), items.end(), BagItemStack::EmptyStack());
	if (emptySlot != items.end())
	{
		return true;
	}
	return false;
}

bool PlayerBag::TryCraft(
    const ItemId _id)
{
	ItemInfo itemInfo      = Get<ItemSystem>().GetInfo(_id);
	const auto& craftInfos = itemInfo._craftInfo;

	// クラフトで消費するアイテム
	struct ItemConsumption
	{
		int slotIndex;
		int useItemCount;
	};
	std::vector<ItemConsumption> stackUsedCraftTmps;

	// クラフトに必要なアイテムが揃っているか確認
	for (const CraftInfo& craftInfo : craftInfos)
	{
		int idx = FindCraftMaterialSlot(craftInfo);
		if (idx == -1)
		{
			// 素材不足：失敗音
			Get<AudioManager>().Play(mistakeSE);
			return false;
		}
		stackUsedCraftTmps.push_back(ItemConsumption{ .slotIndex = idx, .useItemCount = craftInfo._value });
	}

	// 実際に作成(消費を反映)
	for (auto& reservation : stackUsedCraftTmps)
	{
		if (reservation.slotIndex < 0 || reservation.slotIndex >= items.size())
		{
			assert(false);
			return false;
		}

		BagItemStack& slot = items[reservation.slotIndex];
		slot.count -= reservation.useItemCount;

		if (slot.count == 0)
		{
			slot = BagItemStack::EmptyStack();
		}
	}

	Sort();

	// クラフト成功音
	Get<AudioManager>().Play(craftingSE);

	return true;
}

void PlayerBag::AddItem(
    const ItemId _id)
{
	ItemType itemType = Get<ItemSystem>().GetInfo(_id)._type;
	if (itemType == ItemType::Weapons)
		return;

	// 既に同じIDのアイテムが存在する場合は、スタック可能なスロットに追加する

	// 同じIDのアイテムの範囲を取得
	auto [begin, end] = std::equal_range(
	    items.begin(),
	    items.end(),
	    BagItemStack{ .id = _id },
	    [](const BagItemStack& _a, const BagItemStack& _b)
	    {
		    return _a.id < _b.id;
	    });

	// 追加するアイテムの残り個数
	int remain = 1;

	// 同じIDのアイテムのスロットを走査
	for (auto itr = begin; itr != end; itr++)
	{
		if (remain == 0)
		{
			break;
		}

		const int max          = itr->maxStack;
		const int currentCount = itr->count;
		if (itr->maxStack == itr->count)
		{
			continue;
		}

		// スロットのスタック数 + 追加分が 最大スタック数を超える場合は
		// 最大数まで追加して、残りを次のスロットに追加する
		if (currentCount + remain > max)
		{
			itr->count = max;
			remain -= max - currentCount;
		}
		// スロットのスタック数 + 追加分が 最大スタック数を超えないのでそのまま追加
		else
		{
			itr->count += remain;
			remain = 0;
		}
	}

	// 追加するアイテムの残り個数がある場合は、空のスロットに追加する
	if (remain > 0)
	{
		// 空スロットの位置を取得
		auto emptySlotBegin = std::find(items.begin(), items.end(), BagItemStack::EmptyStack());

		// 先頭の空スロットから順に追加していく
		for (auto itr = emptySlotBegin; itr != items.end(); itr++)
		{
			if (remain == 0)
				break;

			itr->id = _id;

			// スロットの最大スタック数を超える場合は、最大数まで追加して残りは次スロットに追加
			if (remain > itr->maxStack)
			{
				itr->count = itr->maxStack;
				remain -= itr->maxStack;
			}
			else
			{
				itr->count += remain;
				remain = 0;
			}
		}
	}
	Sort();
}

std::vector<CraftMaterialRequirement> PlayerBag::GetCraftRequirements(
    const ItemId _craftItemId)
{
	ItemInfo craftItemInfo = Get<ItemSystem>().GetInfo(_craftItemId);
	auto& craftInfos       = craftItemInfo._craftInfo;
	std::vector<CraftMaterialRequirement> craftMaterials;

	for (auto& craftInfo : craftInfos)
	{
		// バッグ内から、アイテムの所持数を計算
		auto range = std::equal_range(
		    items.begin(),
		    items.end(),
		    BagItemStack{ .id = craftInfo._id },
		    [](const BagItemStack& _a, const BagItemStack& _b)
		    {
			    return _a.id < _b.id;
		    });

		int totalOwnCount = std::accumulate(
		    range.first,
		    range.second,
		    0,
		    [](int sum, const BagItemStack& _stack)
		    {
			    return sum + _stack.count;
		    });

		// 足りているか、いくつ持っているかなど返す
		CraftMaterialRequirement material{ .id            = craftInfo._id,
			                               .isSufficient  = (totalOwnCount >= craftInfo._value),
			                               .ownedCount    = totalOwnCount,
			                               .requiredCount = craftInfo._value };
		craftMaterials.push_back(material);
	}

	return craftMaterials;
}

void PlayerBag::ClearItems()
{
	BagItemStack emptyStack = BagItemStack::EmptyStack();
	std::fill(items.begin(), items.end(), emptyStack);
}

bool PlayerBag::DiscardItem(
    const ItemId _id)
{
	auto foundItemStack = std::find_if(
	    items.begin(),
	    items.end(),
	    [_id](const BagItemStack& _itemStack)
	    {
		    return _itemStack.id == _id;
	    });
	// 破棄したいアイテムを持っていない場合
	if (foundItemStack == items.end())
	{
		return false;
	}
	const int DISCARD_COUNT{ 1 };
	// 破棄したい数より所持数が少ない場合
	if (foundItemStack->count < DISCARD_COUNT)
	{
		return false;
	}

	// アイテム数を減らす
	foundItemStack->count -= DISCARD_COUNT;
	// 破棄した結果そのスロットが空になったら、空状態にする
	if (foundItemStack->count == 0)
	{
		foundItemStack->Reset();
		// バッグの中身をソートする
		Sort();
	}

	// バッグ内のアイテムを破棄するのに成功!
	return true;
}

int PlayerBag::GetItemCount(
    const ItemId _id)
{
	int count = 0;
	for (const auto& bagItemStack : items)
	{
		if (_id == bagItemStack.id)
		{
			count += bagItemStack.count;
		}
	}
	return count;
}

void PlayerBag::Sort()
{
	// アイテムIDの昇順でソート、同じIDの場合は個数の降順でソート
	std::sort(
	    items.begin(),
	    items.end(),
	    [](const BagItemStack& _a, const BagItemStack& _b)
	    {
		    if (_a.id == _b.id)
		    {
			    if (_a.count > _b.count)
			    {
				    return true;
			    }
			    else
			    {
				    return false;
			    }
		    }

		    return _a.id < _b.id;
	    });
}

int PlayerBag::FindCraftMaterialSlot(
    const CraftInfo& _craftInfo) const
{
	auto foundItemStack = std::find_if(
	    items.begin(),
	    items.end(),
	    [_craftInfo](const BagItemStack& _itemStack)
	    {
		    return _craftInfo._id == _itemStack.id;
	    });

	// アイテム持ってない
	if (foundItemStack == items.end())
	{
		return -1;
	}

	// 個数が足らない
	if (foundItemStack->count < _craftInfo._value)
	{
		return -1;
	}

	auto idx = std::distance(items.begin(), foundItemStack);
	return static_cast<int>(idx);
}

void BagItemStack::Reset()
{
	id       = EMPTY_STACK_ITEM_ID;
	count    = 0;
	maxStack = 64;
}
