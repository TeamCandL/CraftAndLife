#include "ClearSceneInput.h"
#include <Game/Manager/AudioManager.h>

// TitleScene.hの実際のパスに合わせてください
#include <Game/TitleScene/TitleScene.h>

void ClearSceneInput::Update()
{
	// 経過時間を加算
	elapsedTime += Get<DeltaTime>();

	// 4秒経過するまでは入力を受け付けない
	if (elapsedTime < inputDelay)
	{
		return;
	}

	// Spaceキーが押されたらタイトルへ
	if (Get<InputSystem>().GetKeyDown(KeyCode::Space))
	{
		ManagerRegistry::Ref<AudioManager>().SetBGMStop();

		Get<SceneManager>().Move<TitleScene>();
	}
}