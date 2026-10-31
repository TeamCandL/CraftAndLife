#pragma once
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>

struct SlotItemData
{
	Component<UITransform> transform{};
	Component<UIImage> image{};
	ItemId id{ 0 };
};

struct SlotItem : ObjectBase<SlotItemData>
{
	using ObjectBase<SlotItemData>::ObjectBase;
};

struct SlotData
{
	SlotData(
	    const Resource<Texture> _textureResource,
	    ItemType _itemType) :
	    image{ _textureResource },
	    stackableItemType{ _itemType }
	{
	}

	Component<UITransform> transform{};
	Component<UIImage> image{};
	GameObject slotItem{};
	ItemType stackableItemType{};
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

  private:
	/// @brief
	/// 指定された方向にフォーカスを移動。フォーカスは循環する。一番右のスロットでさらに右を押したら一番左へ行く。
	/// @param _dir フォーカスを移動する方向を指定する値
	void MoveFocusImpl(int _dir);
};
