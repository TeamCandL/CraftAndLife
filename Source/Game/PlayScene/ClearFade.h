#pragma once
#include <Game/Game.h>
#include "Game/ClearScene/ResultType.h"

struct ClearFadeData
{
	// フェード時間
	float fadeDuration{ 3.0f };

	// 経過時間
	float elapsedTime{};

	// フェード中か
	bool isFading{ false };

	// 結果の種類
	ResultType resultType{ ResultType::Clear };

	// フェード画面
	Component<UITransform> transform{};
	Component<UIImage> image{};
};

struct ClearFade : ObjectBase<ClearFadeData>
{
	void Start() override;
	void Update() override;

	// 結果の種類を指定してフェード開始
	void StartFade(ResultType _resultType = ResultType::Clear);
};