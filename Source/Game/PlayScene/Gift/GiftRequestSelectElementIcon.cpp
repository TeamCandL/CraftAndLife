#include "pch\pch.h"
#include "GiftRequestSelectElementIcon.h"

void GiftRequestSelectElementIcon::SetFocus(
    const bool _isFocus)
{
	if (_isFocus)
	{
		transform.position = centerPosition - Vec2{ 0.5f, 0.5f } * focusedScale;
		transform.scale    = Vec2::One() * focusedScale;
	}
	else
	{
		transform.position = centerPosition - Vec2{ 0.5f, 0.5f } * baseScale;
		transform.scale    = Vec2::One() * baseScale;
	}
}
