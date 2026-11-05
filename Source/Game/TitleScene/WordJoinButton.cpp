#include "WordJoinButton.h"
#include <Game/Common/UI/Button.h>
#include "WordInputPanel.h"
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Manager/SessionManager.h>
#include "TitleNetworkController.h"
#include <Game/Manager/AudioManager.h>

void WordJoinButton::Start()
{
	objButton = GetScene().Instantiate<UI::Button>(imageFileName, imageFileName, imageFileName, "", begin, scale);
}

void WordJoinButton::Update()
{
	using enum NetworkSessionManager::State;

	if (isInputtingWord)
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300, 200 }, ImGuiCond_Appearing);

		// 適用するフラグのセット
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |      // 移動不可
		                         ImGuiWindowFlags_NoResize |    // リサイズ不可
		                         ImGuiWindowFlags_NoCollapse |  // 折りたたみ不可
		                         ImGuiWindowFlags_NoTitleBar;

		ImGui::Begin("合言葉入力欄", nullptr, flags);
		ImGui::InputText("##inputWord", &inputWord);
		ImGui::End();

		if (Get<WordInputPanel>(objWordInputPanel).IsShown())
		{
			return;  // 表示されているならそのまま
		}

		if (inputWord.size() > 0)
		{
			// 何らかしらの入力があったらロビーに進むために接続だけする
			NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();
			networkSessionManager.SetCurrentState(Connecting);
			networkSessionManager.Connect();

			Get<SessionManager>().SetJoinRoomName(inputWord);
		}
		isInputtingWord = false;

		Get<AudioManager>().Play(pushSound);
		return;
	}

	if (Get<UI::Button>(objButton).IsPushed())
	{
		Get<WordInputPanel>(objWordInputPanel).Show();
		isInputtingWord = true;

		Get<AudioManager>().Play(pushSound);
	}
}
