#include "ClearSceneInput.h"
#include <Game/Manager/AudioManager.h>
#include <Base/Manager/IssueManager.h>
#include <Game/UI/ImageObject.h>
#include <Game/UI/TextObject.h>

// TitleScene.hの実際のパスに合わせてください
#include <Game/TitleScene/TitleScene.h>

void ClearSceneInput::Start()
{
	objInputBackground = GetScene().Instantiate<ImageObject>(backgroundTexture, Vec2{ 533, 875 }, Vec2{ 1006, 128 }, 2);
	Get<ImageObject>(objInputBackground).SetActive(false);
	FontLoadConfig config;
	config.fontSize = 64;

	Resource<ResourceType::Font> font{ config };
	objInputText = GetScene().Instantiate<TextObject>(Vec2{ 533, 875 }, Vec2{ 964, 93 }, 3, font);
	Get<TextObject>(objInputText).Data().text.content = "Spaceキーを押すと、ゲーム終了";
	Get<TextObject>(objInputText).SetActive(false);
}

void ClearSceneInput::Update()
{
	// 経過時間を加算
	elapsedTime += Get<DeltaTime>();

	// 4秒経過するまでは入力を受け付けない
	if (elapsedTime < inputDelay)
	{
		return;
	}
	Get<ImageObject>(objInputBackground).SetActive(true);
	Get<TextObject>(objInputText).SetActive(true);

	// Spaceキーが押されたらタイトルへ
	if (Get<InputSystem>().GetKeyDown(KeyCode::Space))
	{
		ManagerRegistry::Ref<AudioManager>().SetBGMStop();

		// Get<SceneManager>().Move<TitleScene>();
		Get<IssueManager>().Error(L"ゲーム終了");
	}
}