#include "ClearSceneInput.h"
#include <Game/Manager/AudioManager.h>
#include <Base/Manager/IssueManager.h>
#include <Game/UI/ImageObject.h>
#include <Game/UI/TextObject.h>

// TitleScene.hの実際のパスに合わせてください
#include <Game/TitleScene/TitleScene.h>

void ClearSceneInput::Start()
{
	if (resultType == ResultType::Clear)
	{
		texture       = Resource<Texture>{ "Image/Clear/quit_game_gameclear.png" };
		objInputImage = GetScene().Instantiate<ImageObject>(texture, Vec2{ 497, 702 }, Vec2{ 926, 309 }, 3);
	}
	else
	{
		texture       = Resource<Texture>{ "Image/GAMEOVER/quit_game_gameover.png" };
		objInputImage = GetScene().Instantiate<ImageObject>(texture, Vec2{ 490, 654 }, Vec2{ 926, 309 }, 3);
	}
	Get<TextObject>(objInputImage).SetActive(false);
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
	Get<ImageObject>(objInputImage).SetActive(true);

	// Spaceキーが押されたらタイトルへ
	if (Get<InputSystem>().GetKeyDown(KeyCode::Space))
	{
		ManagerRegistry::Ref<AudioManager>().SetBGMStop();

		Get<IssueManager>().Error(L"ゲーム終了");
	}
}

ClearSceneInputData::ClearSceneInputData(
    ResultType _resultType) :
    resultType{ _resultType }
{
}
