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

	std::array<Resource<Texture>, TutorialImageType_Max> images{
		"Image/UI/Tutorial/1_camera.png",        "Image/UI/Tutorial/2_body.png",
		"Image/UI/Tutorial/3_jump.png",          "Image/UI/Tutorial/4_hotbar.png",
		"Image/UI/Tutorial/5_open_backpack.png", "Image/UI/Tutorial/6_close_backpack.png",
	};

	Component<UITransform> transform{};
	Component<UIImage> image{};
};

struct TutorialUI : ObjectBase<TutorialUIData>
{
	void Start() override;

	void SetImage(const TutorialImageType _type);
};
