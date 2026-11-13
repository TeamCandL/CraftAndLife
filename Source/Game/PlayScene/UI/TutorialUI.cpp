#include "pch\pch.h"
#include "TutorialUI.h"

void TutorialUI::Start()
{
	transform.position   = Vec2::Zero();
	transform.scale      = Get<ScreenSize>();
	transform.layerOrder = UILayerOrder{}.WithUser(100002);
}

void TutorialUI::SetImage(
    const TutorialImageType _type)
{
	if (_type < 0 || TutorialImageType_Max <= _type)
	{
		transform.scale = Vec2::Zero();
	}
	else
	{
		transform.scale = Get<ScreenSize>();
		image.texture   = images[_type];
	}
}
