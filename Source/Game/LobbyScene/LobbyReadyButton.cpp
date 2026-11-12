#include "LobbyReadyButton.h"
#include <Game/Common/UI/Button.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/ReadySelf.h>
#include <Game/Network/Protocols/StartGame.h>
#include <Game/Manager/SessionManager.h>
void LobbyReadyButton::Start()
{

	FontLoadConfig config{};
	config.fontSize = 64;
	config.WithFontAlignment(cha::FontAlignment_::FontAlignment_Center);
	Resource<ResourceType::Font> font{ config };
	objButton = GetScene().Instantiate<UI::Button>(
	    textureNormalFileName,
	    textureSelectedFileName,
	    textureSelectedFileName,
	    "",
	    begin,
	    scale,
	    font);
}

void LobbyReadyButton::Update()
{
	using enum NetworkSessionManager::State;
	auto& status = Get<SessionManager>().RefStatus();
	if (status.imLobbyHost)
	{
		if (isStatable)
		{
			Get<UIText>(objButton).content = "ゲーム開始";
		}
		else
		{
			Get<UIText>(objButton).content = "準備完了とする";
		}
	}
	else
	{
		Get<UIText>(objButton).content = "待機中...";
	}
	if (Get<UI::Button>(objButton).IsPushed())
	{
		isStatable                                   = true;
		NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();
		if (networkSessionManager.GetCurrentState() == InLobbyNotReady)
		{
			networkSessionManager.SetCurrentState(InLobbyWaiting);
			Protocol::ReadySelf message{};
			networkSessionManager.Send(message);
		}
		else
		{
			Protocol::StartGame message{};
			networkSessionManager.Send(message);
		}
	}
}
