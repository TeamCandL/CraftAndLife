#include "ClearFade.h"
#include <Game/ClearScene/ClearScene.h>
#include <Base/Manager/ScreenSize.h>

void ClearFade::Start()
{
	// 白い画面を設定
	image.baseColor = Color{ 1.0f, 1.0f, 1.0f, 0.0f };

	// 画面全体を覆う
	const Vec2 SCREEN_SIZE = Get<ScreenSize>();

	transform.scale    = SCREEN_SIZE;
	transform.position = Vec2::Zero();
}

void ClearFade::Update()
{
	if (!isFading)
	{
		return;
	}

	const float DELTA_TIME = Get<DeltaTime>();

	elapsedTime += DELTA_TIME;

	// 0.0～1.0に制限
	const float RATIO = std::clamp(elapsedTime / fadeDuration, 0.0f, 1.0f);

	// 白色の透明度を徐々に上げる
	image.baseColor.a = RATIO;

	// フェード完了
	if (RATIO >= 1.0f)
	{
		Get<SceneManager>().Move<ClearScene>();
	}
}

void ClearFade::StartFade()
{
	if (isFading)
	{
		return;
	}

	isFading    = true;
	elapsedTime = 0.0f;
}