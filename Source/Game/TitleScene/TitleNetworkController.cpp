#include "TitleNetworkController.h"
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/MessageConverter.h>
#include <Game/Manager/SessionManager.h>
#include <Game/Network/Protocols/JoinRoom.h>
#include <Game/Network/Protocols/JoinOk.h>
#include <Game/Manager/SessionManager.h>
#include "../LobbyScene/LobbyScene.h"

using enum NetworkSessionManager::State;

void TitleNetworkController::Start()
{
	Get<SessionManager>().Reset();  // タイトル来たらセッションはリセットする
	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();
	networkSessionManager.Disconnect();
	networkSessionManager.SetCurrentState(Disconnected);
}

void TitleNetworkController::Update()
{
	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	MessageConverter converter{};

	switch (networkSessionManager.GetCurrentState())
	{
	case Connecting:
		networkSessionManager.Receive(
		    [this, &converter, &networkSessionManager](const ENetTransport::Event& _event)
		    {
			    if (_event.type == ENetTransport::EventType::Connected)
			    {
				    Protocol::JoinRoom message{};
				    message.roomName = Get<SessionManager>().RefStatus().joinRoomName;
				    networkSessionManager.SetCurrentState(CheckJoin);
				    networkSessionManager.Send(message);
			    }
		    });
		break;
	case CheckJoin:
		networkSessionManager.Receive(
		    [this, &converter, &networkSessionManager](const ENetTransport::Event& _event)
		    {
			    ProtocolType protocolType = converter.GetProtocolType(_event.data);

			    switch (protocolType)
			    {
			    case ProtocolType::JoinOK:
			    {
				    Protocol::JoinOK protocol{};
				    converter.ToMessage(_event.data, protocol);

				    VCU_LOG("JoinOK受信");
				    VCU_LOG("プレイヤー名: {}", protocol.playerName);
				    VCU_LOG("プレイヤーID: {}", protocol.playerId);
				    VCU_LOG("ロールID: {}", protocol.roleId);

				    SessionManager& sessionManager = Get<SessionManager>();

				    sessionManager.SetPlayerName(protocol.playerName);
				    sessionManager.SetPlayerRole(protocol.playerId, protocol.roleId);

				    // Get<SessionManager>().SetPlayerName(protocol.playerName);
				    Get<SceneManager>().Move<LobbyScene>();
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
