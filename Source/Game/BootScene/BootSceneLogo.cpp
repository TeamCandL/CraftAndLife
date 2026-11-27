#include "BootSceneLogo.h"
#include <Game/MapTestScene/MapTestScene.h>
#include <Game/PlayScene/PlayScene.h>
#include <Game/TitleScene/TitleScene.h>
#include <Game/ModelTestScene/ModelTestScene.h>

void BootSceneLogo::Start()
{
	const Vec2 SCREEN_SIZE = Get<ScreenSize>();
	transform.position     = (SCREEN_SIZE - transform.scale) * 0.5f;
	image.texture          = texture;
}

void BootSceneLogo::Update()
{
	image.baseColor = Color::FromWebCodeA(0xffffff00);

	timeLeftSec -= Get<DeltaTime>();

	float last = timeLeftSec;

	if (last <= 0.0f)
	{
		// ロゴの表示が終わったためシーン遷移する
		Get<SceneManager>().Move<TitleScene>();
		// Get<SceneManager>().Move<ModelTestScene>();
		//     Get<SceneManager>().Move<MapTestScene>();
		//   Get<SceneManager>().Move<PlayScene>();
		return;
	}

	bool isOnSkip = Get<RawMouseInputManager>().GetMouseDown(MouseCode_Left);

	last -= fadeOutTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f - -last / fadeOutTime;

		if (isOnSkip)
		{
			timeLeftSec = 0.0f;
		}
		return;
	}

	last -= stayTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f;

		if (isOnSkip)
		{
			timeLeftSec = fadeOutTime;
		}
		return;
	}

	last -= fadeInTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = -last / fadeOutTime;

		if (isOnSkip)
		{
			timeLeftSec = fadeOutTime + stayTime;
		}
		return;
	}
}
