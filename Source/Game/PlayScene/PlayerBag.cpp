#include "PlayerBag.h"
#include <numeric>
namespace
{
	// テスト用のクラフトアイテム
	ItemInfo woodAllInfo{ ._id        = 1,
		                  ._type      = ItemType::Materials,
		                  ._name      = "Wood",
		                  ._craftInfo = { { ._id = 3, ._value = 2 }, { ._id = 4, ._value = 4 } } };
}  // namespace
PlayerBagData::PlayerBagData()
{
	items.resize(initialCapacity, ItemStack::EmptyStack());
}

void PlayerBag::Start() {}

void PlayerBag::Update()
{
	static ItemId id{ 0 };
	static int count{ 1 };

	// デバッグ用のUI
	ImGui::Begin("Bag");
	if (ImGui::Button("ExpansiionCapa"))
	{
		ExpansionCapacity(10);
	}
	if (ImGui::Button("Craft"))
	{
		Craft(woodAllInfo._id);
	}
	auto requirements = GetCraftRequirements(woodAllInfo._id);
	for (auto& requirement : requirements)
	{
		ImGui::Text("%d Id", requirement.id);
		ImGui::Checkbox("IsSuffice", &requirement.isSufficient);
		ImGui::Text("%d Owned", requirement.ownedCount);
		ImGui::Text("%d Required", requirement.requiredCount);
	}

	// アイテム追加用のUI(デバッグ)
	ImGui::InputInt("ID", &id);
	ImGui::InputInt("Count", &count);
	if (ImGui::Button("Add Item"))
	{
		AddItem(id, count);
	}

	for (int i = 0; i < items.size(); i++)
	{
		if (items[i].id == EMPTY_ITEM_ID)
		{
			ImGui::Text("%d: empty, %d", i, items[i].count);
		}
		else
		{
			ImGui::Text("%d: %d, %d", i, items[i].id, items[i].count);
		}
	}
	ImGui::End();
}

void PlayerBag::ExpansionCapacity(
    const unsigned _capacity)
{
	// バッグの容量を拡張。新規スロットは空の状態で追加する
	items.resize(items.size() + _capacity, ItemStack::EmptyStack());
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

void PlayerBag::Craft(
    const ItemId _id)
{
	ItemInfo itemInfo = woodAllInfo;
	// ItemInfo itemInfo      = Get<ItemSystem>().GetInfo(_id);
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
			// assert(false);
			return;
		}
		stackUsedCraftTmps.push_back(ItemConsumption{ .slotIndex = idx, .useItemCount = craftInfo._value });
	}

	// 実際に作成(消費を反映)
	for (auto& reservation : stackUsedCraftTmps)
	{
		if (reservation.slotIndex < 0 || reservation.slotIndex >= items.size())
		{
			// assert(false);
			return;
		}
		ItemStack& slot = items[reservation.slotIndex];
		slot.count -= reservation.useItemCount;

		if (slot.count == 0)
		{
			slot = ItemStack::EmptyStack();
		}
	}
	// クラフトによって作成されるアイテム数は1個
	AddItem(_id, 1);

	Sort();
}

void PlayerBag::AddItem(
    const ItemId _id,
    const unsigned _count)
{
	// 既に同じIDのアイテムが存在する場合は、スタック可能なスロットに追加する

	// 同じIDのアイテムの範囲を取得
	auto [begin, end] = std::equal_range(
	    items.begin(),
	    items.end(),
	    ItemStack{ .id = _id },
	    [](const ItemStack& _a, const ItemStack& _b)
	    {
		    return _a.id < _b.id;
	    });

	// 追加するアイテムの残り個数
	int remain = _count;

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
		auto emptySlotBegin = std::find(items.begin(), items.end(), ItemStack::EmptyStack());
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
	ItemInfo craftItemInfo = woodAllInfo;
	// ItemInfo craftItemInfo = Get<ItemSystem>().GetInfo(_craftItemId);
	auto& craftInfos = craftItemInfo._craftInfo;
	std::vector<CraftMaterialRequirement> craftMaterials;

	for (auto& craftInfo : craftInfos)
	{
		// バッグ内から、アイテムの所持数を計算
		auto range = std::equal_range(
		    items.begin(),
		    items.end(),
		    ItemStack{ .id = craftInfo._id },
		    [](const ItemStack& _a, const ItemStack& _b)
		    {
			    return _a.id < _b.id;
		    });

		int totalOwnCount = std::accumulate(
		    range.first,
		    range.second,
		    0,
		    [](int sum, const ItemStack& _stack)
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

void PlayerBag::Sort()
{
	// アイテムIDの昇順でソート、同じIDの場合は個数の降順でソート
	std::sort(
	    items.begin(),
	    items.end(),
	    [](const ItemStack& _a, const ItemStack& _b)
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
	    [_craftInfo](const ItemStack& _itemStack)
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
