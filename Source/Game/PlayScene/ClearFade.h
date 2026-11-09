#pragma once
#include <Game/Game.h>

struct ClearFadeData
{
	// フェード時間
	float fadeDuration{ 3.0f };

	// 経過時間
	float elapsedTime{};

	// フェード中か
	bool isFading{ false };

	// 白い画面
	Component<UITransform> transform{};
	Component<UIImage> image{};
};

struct ClearFade : ObjectBase<ClearFadeData>
{
	void Start() override;
	void Update() override;

	void StartFade();
};