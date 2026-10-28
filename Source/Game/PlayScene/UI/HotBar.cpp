#include "pch\pch.h"
#include "HotBar.h"
#include "MiniMapPoint.h"
void HotBar::Start()
{
	for (int i = 0; i < slots.size(); i++)
	{
		slots[i]                      = GetScene().Instantiate<Slot>(slotEmptyTexture);
		SlotData& slotData            = Get<Slot>(slots[i]).Data();
		slotData.transform.layerOrder = 1;
		slotData.transform.scale      = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		slotData.selected             = (currentSlotIdx == i);
	}
}

void HotBar::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::I))
	{
		MoveFocusToLeft();
	}
	if (Get<InputSystem>().GetKeyDown(KeyCode::O))
	{
		MoveFocusToRight();
	}

	for (int i = 0; i < slots.size(); i++)
	{
		SlotData& slotData      = Get<Slot>(slots[i]).Data();
		GameObject& slotGameObj = slots[i];
		// i番目のスロット描画位置のオフセット
		const int offset = (slotMargin + slotEmptyImageExtents) * i;
		// スロットの中心座標
		const Vec2 center = { transform.position.x + leftSlotCenter.x + offset,
			                  transform.position.y + leftSlotCenter.y };
		// スロットが選択されているか判定
		slotData.selected = (currentSlotIdx == i);

		// 選択状態によって画像、座標、サイズを変える
		if (slotData.selected)
		{
			Get<UIImage>(slotGameObj).texture = slotSelectedTexture;
			const int extentsHalf             = slotSelectedImageExtents / 2;
			slotData.transform.position       = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			slotData.transform.scale          = Vec2{ slotSelectedImageExtents, slotSelectedImageExtents };
		}
		else
		{
			Get<UIImage>(slotGameObj).texture = slotEmptyTexture;
			const int extentsHalf             = slotEmptyImageExtents / 2;
			slotData.transform.position       = Vec2{ center.x - extentsHalf, center.y - extentsHalf };
			slotData.transform.scale          = Vec2{ slotEmptyImageExtents, slotEmptyImageExtents };
		}
		// NOTE: ゲームループによる更新順番の関係上、ここでワールド行列を計算する必要あり
		Get<Slot>(slotGameObj).CalculateUIWorld();
	}
}

void HotBar::MoveFocusToRight()
{
	MoveFocusImpl(1);
}

void HotBar::MoveFocusToLeft()
{
	MoveFocusImpl(-1);
}

void HotBar::MoveFocusImpl(
    int _dir)
{
	currentSlotIdx += _dir;
	currentSlotIdx = (currentSlotIdx + slots.size()) % slots.size();
}

void Slot::CalculateUIWorld()
{
	using namespace DirectX;
	transform.uiWorld = transform.uiLocal =
	    XMMatrixIdentity() * DirectX::XMMatrixScaling(transform.scale.x, transform.scale.y, 1.0f) *
	    DirectX::XMMatrixTranslation(transform.position.x, transform.position.y, 0.0f);
}
