#include "ClearSceneTrigger.h"
#include <Game/ClearScene/ClearScene.h>

void ClearSceneTrigger::Update()
{
	// Kキーが押されたらクリアシーンへ移動
	if (Get<InputSystem>().GetKeyDown(KeyCode::Enter))
	{
		Get<SceneManager>().Move<ClearScene>();
	}
}