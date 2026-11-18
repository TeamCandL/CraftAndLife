// TheGame.cpp : アプリケーションのエントリ ポイントを定義します。

#include "TheGame.h"
#include <Base/Base.h>
#include <Base/GameLoop.h>

/// @brief ゲームのエントリポイント
void Main(
    const HINSTANCE _hInstance)
{
	cha::GameLoop gameLoop{ {
#if _DEBUG
		.clientSize = { 1920, 1080 },
		.targetFPS  = 60,
		.isWindow   = true,
#elif 1
		.clientSize = { 1920, 1080 },
		.targetFPS  = 60,
		.isWindow   = false,
#endif
	} };

	gameLoop.Run(_hInstance);
}
