#pragma once
#include <Game/Game.h>

struct SlotData
{
	SlotData(
	    const Resource<Texture> _textureResource) :
	    image{ _textureResource }
	{
	}

	Component<UITransform> transform{};
	Component<UIImage> image{};
	bool selected{ false };
};
struct Slot : ObjectBase<SlotData>
{
	using ObjectBase<SlotData>::ObjectBase;
};

struct HotBarData
{
	Resource<Texture> texture{ "Image/UI/slot_panel.png" };
	Resource<Texture> slotEmpty{ "Image/UI/slot_empty.png" };
	Resource<Texture> slotSelected{ "Image/UI/slot_selected.png" };
	Component<UITransform> transform{ Vec2{ 1528, 908 }, Vec2{ 350, 128 } };
	Component<UIImage> image{ texture };

	GameObject leftSlot;

	Vec2 leftSlotTop{ 22, 19 };
	int slotEmptyImageExtents{ 90 };
	int slotSelectedImageExtents{ 110 };
	int slotMargin{ 18 };
};

struct HotBar : ObjectBase<HotBarData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
