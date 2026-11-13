#pragma once
#include <Game/Game.h>

struct TutorialUIData
{
	using TutorialImageType = int32_t;
	enum TutorialImageType_ : TutorialImageType
	{
		TutorialImageType_Camera,
		TutorialImageType_Body,
		TutorialImageType_Jump,
		TutorialImageType_Hotbar,
		TutorialImageType_OpenBackpack,
		TutorialImageType_CloseBackpack,
		TutorialImageType_Max,
	};

	Component<UITransform> transform{};
	std::array<Component<UIImage>, TutorialImageType_Max> images{};
};

struct TutorialUI : ObjectBase<TutorialUIData>
{
	void Start() override;
};
