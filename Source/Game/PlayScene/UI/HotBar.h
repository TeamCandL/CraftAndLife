#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>

struct SlotItemCountData
{
	Component<UITransform> transform{};
	Component<UIText> text{};
};
struct SlotItemCount : ObjectBase<SlotItemCountData>
{
	using ObjectBase<SlotItemCountData>::ObjectBase;
};
struct SlotItemData
{
	ItemId id{ 0 };
	int count{ 0 };

	Component<UITransform> transform{};
	Component<UIImage> image{};
	GameObject objItemCount{};
};

struct SlotItem : ObjectBase<SlotItemData>
{
	using ObjectBase<SlotItemData>::ObjectBase;
	void Reset();
};

struct SlotData
{
	SlotData(
	    const Resource<Texture> _textureResource) :
	    image{ _textureResource }
	{
	}

	Component<UITransform> transform{};
	Component<UIImage> image{};
	GameObject slotItem{};
	bool selected{ false };
};
struct Slot : ObjectBase<SlotData>
{
	using ObjectBase<SlotData>::ObjectBase;
	void CalculateUIWorld();
};

struct HotBarData
{
	// パネルの画像リソース
	Resource<Texture> panelTexture{ "Image/UI/slot_panel.png" };
	// 非選択時のスロット画像
	Resource<Texture> slotEmptyTexture{ "Image/UI/slot_empty.png" };
	// 選択時のスロット画像
	Resource<Texture> slotSelectedTexture{ "Image/UI/slot_selected.png" };

	// パネルのトランスフォーム
	Component<UITransform> transform{ Vec2{ 1528, 908 }, Vec2{ 350, 128 } };
	// パネルの画像
	Component<UIImage> image{ panelTexture };

	// スロット数
	static constexpr int SLOT_COUNT{ 3 };
	std::array<GameObject, 3> slots{};
	// 現在選択されているスロットのインデックス
	int currentSlotIdx{ 0 };

	// 左のスロットの中心座標(パネルとの相対座標)
	Vec2 leftSlotCenter{ 67, 64 };
	// 非選択時のスロット画像のサイズ
	int slotEmptyImageExtents{ 90 };
	// 選択時のスロット画像のサイズ
	int slotSelectedImageExtents{ 110 };
	// スロット画像間の隙間
	int slotMargin{ 18 };
	// 左のスロットのアイテム数の座標(パネルとの相対座標)
	Vec2 leftSlotItemCountBegin{ 67, 58 };
};

struct HotBar : ObjectBase<HotBarData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief スロットのフォーカスを右に進める
	void MoveFocusToRight();
	/// @brief スロットのフォーカスを左に進める
	void MoveFocusToLeft();
	/// @brief 現在選択中のスロットのアイテムを返す
	/// @return
	ItemId GetSelectedItemId();
	bool CanRemoveSelectedItem(int _count = 1);
	void RemoveSelectedItem(int _count = 1);
	/// @brief 消費アイテム(または矢)のIDを返す
	/// @return
	ItemId GetConsumableItemId();
	/// @brief 消費アイテム(または矢)を、指定された個数だけ除外できるか判定
	/// @param _count 指定するアイテム数
	/// @return
	bool CanRemoveConsumableItem(int _count = 1);
	/// @brief
	/// 消費アイテム(または矢)を、指定された個数だけ除外。足りない場合は一つも除外されない。個数が0になった場合、スロットは空になる。
	/// @param _count
	void RemoveConsumableItem(int _count = 1);
	/// @brief ホットバーにアイテムの追加を試みる。アイテムの種類に応じて、どのスロットに入れるかは自動で決める
	/// @param _id
	/// @return 追加できた場合はtrue
	bool TrySetItem(ItemId _id);

	/// @brief スロットに存在するアイテムと、引数のアイテムを入れ替える。
	/// @param _newItemId
	/// @param _slotItemType
	/// @return もともとスロットにあったアイテムのID。入れ替えに失敗した場合、空のアイテムIDを返す
	bool SwapWeaponItem(ItemId& _oldItemId, ItemId _newItemId);
	bool SwapConsumableItem(ItemStack& _oldItemStack, ItemId _newItemId);
	bool SwapItem(ItemStack& _oldItemStack, ItemId _newItemId);

  private:
	bool CanRemoveItem(ItemType _itemType, int _count = 0);
	void RemoveItem(ItemType _itemType, int _count = 0);
	bool SwapItemImpl(ItemStack& _oldItemStack, ItemId _newItemId, int _index);
	/// @brief
	/// 指定された方向にフォーカスを移動。フォーカスは循環する。一番右のスロットでさらに右を押したら一番左へ行く。
	/// @param _dir フォーカスを移動する方向を指定する値
	void MoveFocusImpl(int _dir);
	SlotItemData& GetSlotItemData(int _idx);
	size_t GetSlotIndexFrom(ItemType _itemType);
};
