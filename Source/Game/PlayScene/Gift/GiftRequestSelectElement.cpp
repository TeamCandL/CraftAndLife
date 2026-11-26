#include "pch\pch.h"
#include "GiftRequestSelectElement.h"

void GiftRequestSelectElement::Start() {}

void GiftRequestSelectElement::Update() {}

void GiftRequestSelectElement::SetFocus(
    const bool _focused)
{
	if (_focused)
	{
		image.addColor = Color{ 0.1f, 0.1f, 0.1f, 0.0f };  // ちょっと明るくする
	}
	else
	{
		image.addColor = Color::CLEAR;
	}
}
