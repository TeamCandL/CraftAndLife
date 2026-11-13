#include "PlayerBag.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Manager/AudioManager.h>

#include <numeric>

namespace
{
	bool CannotStack(
	    ItemType _type)
	{
		switch (_type)
		{
		case ItemType::Shield:
		case ItemType::Weapons:
		case ItemType::WeaponBow:
		case ItemType::Tools:
		case ItemType::Backpack:
			return true;
		default:
			return false;
		}
		return false;
	}
}  // namespace
PlayerBagData::PlayerBagData() {}

PlayerBag::PlayerBag() {}

void PlayerBag::Start()
{
	items.resize(initialCapacity);
	for (int i = 0; i < items.size(); i++)
	{
		items[i].CreateUI(GetScene());
	}
}

void PlayerBag::Update()
{
	static ItemId id{ 0 };
	auto& status = Get<SessionManager>().RefStatus();
	UpdateUI();
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
	size_t sizePrev = items.size();
	// バッグの容量を拡張。新規スロットは空の状態で追加する
	items.resize(items.size() + _capacity);
	for (int i = sizePrev - 1; i < items.size(); i++)
	{
		ResetItemStack(items[i]);
	}
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
	auto emptySlot = std::find(items.begin(), items.end(), BagItemStack{});
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
			slot.Reset();
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
		BagItemStack emptyStack{};
		// 空スロットの位置を取得
		auto emptySlotBegin = std::find(items.begin(), items.end(), emptyStack);

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
	// BagItemStack emptyStack = BagItemStack::EmptyStack();
	for (BagItemStack& stack : items)
	{
		ResetItemStack(stack);
	}
}

void PlayerBag::UpdateUI()
{
	for (int i = 0; i < items.size(); i++)
	{
		const int offset  = slotEmptyImageExtents * i;
		const Vec2 center = { begin.x + stackCenter.x + offset, begin.y + stackCenter.y };
		// TODO: スロットが選択されているか

		// 選択状態によって画像を変える
		auto& item = items[i];
		if (item.selected)
		{
			Get<UIImage>(item.backgroundImage).texture = stackSelectedBackgroundTexture;
			UITransform& backgroundTransform           = Get<UITransform>(item.backgroundImage);
			const int extentsHalf                      = slotSelectedImageExtents / 2;
			backgroundTransform.position               = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			backgroundTransform.scale                  = Vec2{ slotSelectedImageExtents, slotSelectedImageExtents };
		}
		else
		{
			Get<UIImage>(item.backgroundImage).texture = stackBackgroundTexture;
			UITransform& backgroundTransform           = Get<UITransform>(item.backgroundImage);
			const int extentsHalf                      = slotEmptyImageExtents / 2;
			backgroundTransform.position               = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			backgroundTransform.scale                  = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		}

		// NOTE: ゲームループによる更新順番の関係上、ここでワールド行列を計算する必要あり
		CalculateUIWorld(item);
		ItemId id                      = item.id == BagItemStack::EMPTY_STACK_ITEM_ID ? EMPTY_ITEM_ID : item.id;
		Resource<Texture> itemImageRes = Get<ItemSystem>().GetInfo(id)._imageId;
		Get<UIImage>(item.itemIconImage).texture = itemImageRes;

		constexpr int itemSize{ 64 };
		const int itemSizeHalf{ itemSize / 2 };
		UITransform& transform = Get<UITransform>(item.itemIconImage);
		transform.position     = Vec2{ center.x - itemSizeHalf, center.y - itemSizeHalf };
		transform.scale        = Vec2{ itemSize, itemSize };

		// スロットのアイテム数のテキスト
		std::string itemCountStr = "";
		ItemType type            = Get<ItemSystem>().GetInfo(id)._type;

		if (CannotStack(type) == false && id != EMPTY_ITEM_ID)
		{
			itemCountStr = std::to_string(item.count);
		}
		Get<UITransform>(item.itemCountText).position = center;
		Get<UIText>(item.itemCountText).content       = itemCountStr;
	}
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

void PlayerBag::ResetItemStack(
    BagItemStack& _stack)
{
	_stack.id                                    = BagItemStack::EMPTY_STACK_ITEM_ID;
	_stack.count                                 = 0;
	_stack.maxStack                              = 64;
	Get<UIImage>(_stack.backgroundImage).texture = stackBackgroundTexture;
	Get<UIImage>(_stack.itemIconImage).texture   = emptyItemIconTexture;
	Get<UIText>(_stack.itemCountText).content    = "";
}

void PlayerBag::CalculateUIWorld(
    BagItemStack& _stack)
{
	using namespace DirectX;
	UITransform& transform = Get<UITransform>(_stack.backgroundImage);
	transform.uiWorld      = transform.uiLocal =
	    XMMatrixIdentity() * DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f) *
	    DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, 0.0f);
}

void BagItemStack::Reset()
{
	id       = EMPTY_STACK_ITEM_ID;
	count    = 0;
	maxStack = 64;
}

void BagItemStack::CreateUI(
    SceneBase& _sceneBase)
{
	backgroundImage = _sceneBase.Instantiate<ImageObject>();
	itemIconImage   = _sceneBase.Instantiate<ImageObject>();
	itemCountText   = _sceneBase.Instantiate<TextObject>();
}
