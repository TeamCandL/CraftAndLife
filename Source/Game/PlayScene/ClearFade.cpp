#include "ClearFade.h"
#include <Game/ClearScene/ClearScene.h>
#include <Base/Manager/ScreenSize.h>
#include <Game/Manager/AudioManager.h>

void ClearFade::Start()
{
	// クリアフェード開始状態をリセット
	isClearFadeStarted = false;

	// 結果の種類に応じてフェードの色を設定
	if (resultType == ResultType::Clear)
	{
		// 白色
		image.baseColor = Color{ 1.0f, 1.0f, 1.0f, 0.0f };
	}
	else
	{
		// 黒色
		image.baseColor = Color{ 0.0f, 0.0f, 0.0f, 0.0f };
	}

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

	// 透明度を徐々に上げる
	image.baseColor.a = RATIO;

	// フェード完了
	if (RATIO >= 1.0f)
	{
		// クリアの場合は撃破BGMが終わるまで待つ
		if (resultType == ResultType::Clear)
		{
			if (Get<AudioManager>().IsBGMPlaying())
			{
				return;
			}
		}

		Get<SceneManager>().Move<ClearScene>(resultType);
	}
}

void ClearFade::StartFade(
    ResultType _resultType)
{
	// すでにフェード中の場合
	if (isFading)
	{
		// ゲームオーバー以外への変更は受け付けない
		if (_resultType != ResultType::GameOver)
		{
			return;
		}

		// すでにゲームオーバーなら何もしない
		if (resultType == ResultType::GameOver)
		{
			return;
		}
	}

	// クリアフェード中かどうかを設定
	isClearFadeStarted = (_resultType == ResultType::Clear);

	// 結果の種類を保存
	resultType = _resultType;

	// 色を設定
	if (resultType == ResultType::Clear)
	{
		image.baseColor.r = 1.0f;
		image.baseColor.g = 1.0f;
		image.baseColor.b = 1.0f;
	}
	else
	{
		image.baseColor.r = 0.0f;
		image.baseColor.g = 0.0f;
		image.baseColor.b = 0.0f;
	}

	isFading    = true;
	elapsedTime = 0.0f;
}