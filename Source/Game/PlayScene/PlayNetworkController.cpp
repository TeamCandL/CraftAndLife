#include "PlayNetworkController.h"
#include "../TitleScene/TitleScene.h"
#include "IMainCamera.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdatePlayer.h>
#include <Game/Network/Protocols/UpdatedPlayers.h>

using enum NetworkSessionManager::State;

void PlayNetworkController::Start()
{
	Get<NetworkSessionManager>().SetCurrentState(Playing);
}

void PlayNetworkController::Update()
{
	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	MessageConverter converter{};

	switch (networkSessionManager.GetCurrentState())
	{
	case Playing:
	{
		Transform& playerTransform = Get<Transform>(objPlayer);
		IMainCamera& mainCamera = Get<IMainCamera>(objCamera);

		Protocol::UpdatePlayer message{};
		message.positionX = playerTransform.position.x;
		message.positionY = playerTransform.position.y;
		message.positionZ = playerTransform.position.z;

		message.rotationX = mainCamera.GetAngleX();
		message.rotationY = mainCamera.GetAngleY();

		networkSessionManager.Send(message);
		VCU_LOG("更新処理送信");
	}
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
			    case ProtocolType::UpdatedPlayers:
			    {
				    Protocol::UpdatedPlayers message{};
				    converter.ToMessage(_event.data, message);

				    Get<SessionManager>().UpdatePlayers(message.playerNames, message.positions, message.rotations);

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
