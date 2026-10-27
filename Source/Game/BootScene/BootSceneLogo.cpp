#include "BootSceneLogo.h"
#include <Game/MapTestScene/MapTestScene.h>
#include <Game/PlayScene/PlayScene.h>
#include <Game/TitleScene/TitleScene.h>

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
		Get<SceneManager>().Move<PlayScene>();
		// Get<SceneManager>().Move<MapTestScene>();
		// Get<SceneManager>().Move<PlayScene>();
		return;
	}

	ImGui::Begin("BootSceneLogo");
	ImGui::InputFloat("last", &last);

	last -= fadeOutTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f - -last / fadeOutTime;
		ImGui::Text("fadeOutTime");
		ImGui::InputFloat("image.baseColor.a", &image.baseColor.a);
		ImGui::End();
		return;
	}

	last -= stayTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = 1.0f;
		ImGui::Text("stayTime");
		ImGui::InputFloat("image.baseColor.a", &image.baseColor.a);
		ImGui::End();
		return;
	}

	last -= fadeInTime;
	if (last <= 0.0f)
	{
		image.baseColor.a = -last / fadeOutTime;
		ImGui::Text("fadeInTime");
		ImGui::InputFloat("image.baseColor.a", &image.baseColor.a);
		ImGui::End();
		return;
	}

	ImGui::End();
}
