#include "LobbyNetworkController.h"
#include "../PlayScene/PlayScene.h"
#include "../TitleScene/TitleScene.h"
#include "MemberList.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/MessageConverter.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/LobbyMembers.h>
#include <Game/Network/Protocols/StartedGame.h>

using enum NetworkSessionManager::State;

void LobbyNetworkController::Start()
{
	Get<NetworkSessionManager>().SetCurrentState(InLobbyNotReady);
}

void LobbyNetworkController::Update()
{
	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	MessageConverter converter{};

	switch (networkSessionManager.GetCurrentState())
	{
	case InLobbyNotReady:
	case InLobbyWaiting:
		networkSessionManager.Receive(
		    [this, &converter, &networkSessionManager](const ENetTransport::Event& _event)
		    {
			    if (_event.type == ENetTransport::EventType::Disconnected)
			    {
				    Get<SceneManager>().Move<TitleScene>();
				    return; // 通信切断
			    }

			    ProtocolType protocolType = converter.GetProtocolType(_event.data);

			    switch (protocolType)
			    {
			    case ProtocolType::LobbyMembers:
			    {
				    Protocol::LobbyMembers protocol{};
				    converter.ToMessage(_event.data, protocol);

				    Get<SessionManager>().SetMemberNames(protocol.names);

				    // メンバーリスト更新されたよ！！
				    Get<MemberList>(objMemberList).UpdateMemberList();
				    break;
			    }
			    case ProtocolType::StartedGame:
			    {
				    Protocol::StartedGame protocol{};
				    converter.ToMessage(_event.data, protocol);

				    Get<SessionManager>().SetLobbyHost(protocol.imLobbyHost);

				    Get<SceneManager>().Move<PlayScene>();
				    break;
			    }
			    default:
				    VCU_LOG("想定外の謎プロトコルを取得:{}", static_cast<int>(protocolType));
				    break;
			    }
		    });
		break;
	default:
		break;
	}
}
