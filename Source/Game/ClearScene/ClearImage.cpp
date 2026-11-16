#include "ClearImage.h"

void ClearImage::Start()
{
	image.texture = texture;

	// 画面サイズを取得
	const Vec2 SCREEN_SIZE = Get<ScreenSize>();

	// 画面全体に表示
	transform.scale    = SCREEN_SIZE;
	transform.position = Vec2::Zero();
}