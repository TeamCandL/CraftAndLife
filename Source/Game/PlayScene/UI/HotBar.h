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
	void CalculateUIWorld();
};

struct HotBarData
{
	Resource<Texture> panelTexture{ "Image/UI/slot_panel.png" };
	Resource<Texture> slotEmptyTexture{ "Image/UI/slot_empty.png" };
	Resource<Texture> slotSelectedTexture{ "Image/UI/slot_selected.png" };
	Component<UITransform> transform{ Vec2{ 1528, 908 }, Vec2{ 350, 128 } };
	Component<UIImage> image{ panelTexture };

	static constexpr int SLOT_COUNT{ 3 };
	std::array<GameObject, 3> slots{};
	int currentSlotIdx{ 0 };

	Vec2 leftSlotCenter{ 67, 64 };
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
	void MoveFocusToRight();
	void MoveFocusToLeft();

  private:
	void MoveFocusImpl(int _dir);
};
