#include "LobbyReadyButton.h"
#include <Game/Common/UI/Button.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/ReadySelf.h>
#include <Game/Network/Protocols/StartGame.h>


void LobbyReadyButton::Start()
{
	objButton = GetScene().Instantiate<UI::Button>(
		textureReadyFileName,
		textureReadyFileName,
		textureReadyFileName,
		"",
		begin,
		scale);
}

void LobbyReadyButton::Update()
{
	using enum NetworkSessionManager::State;

	if (Get<UI::Button>(objButton).IsPushed())
	{
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
