#include "PlayNetworkController.h"
#include "../TitleScene/TitleScene.h"
#include "IMainCamera.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdatePlayer.h>
#include <Game/Network/Protocols/UpdatedPlayers.h>
#include <Game/Network/Protocols/UpdateEnemy.h>
#include <Game/Network/Protocols/UpdatedEnemies.h>
#include <Game/PlayScene/Enemy.h>
#include <Game/PlayScene/EnemyController.h>
#include "NetworkActorServer.h"
#include <unordered_set>

using enum NetworkSessionManager::State;

void PlayNetworkController::Start()
{
	Get<NetworkSessionManager>().SetCurrentState(Playing);
}

void PlayNetworkController::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::R))
	{
		Get<SceneManager>().Move<TitleScene>();
	}

	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	MessageConverter converter{};

	switch (networkSessionManager.GetCurrentState())
	{
	case Playing:
	{
		// 受信処理を先に行い、受信バッファの蓄積を解消しておく
		networkSessionManager.Receive(
		    [this, &converter, &networkSessionManager](const ENetTransport::Event& _event)
		    {
			    if (_event.type == ENetTransport::EventType::Disconnected)
			    {
				    Get<SceneManager>().Move<TitleScene>();
				    return;  // 通信切断
			    }

			    ProtocolType protocolType = converter.GetProtocolType(_event.data);

			    switch (protocolType)
			    {
			    case ProtocolType::UpdatedPlayers:
			    {
				    Protocol::UpdatedPlayers message{};
				    converter.ToMessage(_event.data, message);

				    Get<SessionManager>().UpdatePlayers(message.playerNames, message.positions, message.rotations);

				    VCU_LOG("Receive UpdatedPlayers:{}", message.flag);
				    break;
			    }
			    case ProtocolType::UpdatedEnemies:
			    {
				    VCU_LOG("UpdatedEnemiesを受信した！");
				    Protocol::UpdatedEnemies enemyMessage{};
				    converter.ToMessage(_event.data, enemyMessage);

				    // 受信した敵データの数を確認
				    VCU_LOG("敵ID数: {}", enemyMessage.enemyIds.Size());
				    VCU_LOG("敵種類数: {}", enemyMessage.enemyTypes.Size());
				    VCU_LOG("座標データ数: {}", enemyMessage.positions.Size());
				    VCU_LOG("回転データ数: {}", enemyMessage.rotations.Size());

				    Get<SessionManager>().UpdateEnemies(
				        enemyMessage.enemyIds,
				        enemyMessage.enemyTypes,
				        enemyMessage.positions,
				        enemyMessage.rotations);

				    break;
			    }
			    default:
				    VCU_LOG("想定外の謎プロトコルを取得:{}", static_cast<int>(protocolType));
				    break;
			    }
		    });

		Transform& playerTransform = Get<Transform>(objPlayer);
		IMainCamera& mainCamera    = Get<IMainCamera>(objCamera);

		Protocol::UpdatePlayer message{};
		message.positionX = playerTransform.position.x;
		message.positionY = playerTransform.position.y;
		message.positionZ = playerTransform.position.z;

		message.rotationX = mainCamera.GetAngleX();
		message.rotationY = mainCamera.GetAngleY();

		static int32_t counter{};
		message.flag = counter++;

		// VCU_LOG("更新処理送信: {}", counter);

		networkSessionManager.Send(message);

		NetworkActorServer& actorServer = Get<NetworkActorServer>(objNetworkActorServer);

		if (actorServer.IsHost() && actorServer.IsInitialized())
		{
			GameObject objEnemyController = actorServer.GetEnemyController();
			auto enemyStatuses            = Get<EnemyController>(objEnemyController).GetAllEnemyStatus();

			// 送信済みの敵IDを記録する
			std::unordered_set<int> enemyIds;

			VCU_LOG("ホスト呼ばれたよ!!!!!!!!！");

			for (const EnemyStatus& enemy : enemyStatuses)
			{
				// 同じIDがすでに登録されていたら送信しない
				if (!enemyIds.insert(enemy.enemyId).second)
				{
					VCU_LOG("敵IDが重複しています: {}", enemy.enemyId);
					continue;
				}

				Protocol::UpdateEnemy enemyMessage{};

				enemyMessage.enemyId   = enemy.enemyId;
				enemyMessage.enemyType = static_cast<int>(enemy.enemyType);

				enemyMessage.positionX = enemy.enemyPosition.x;
				enemyMessage.positionY = enemy.enemyPosition.y;
				enemyMessage.positionZ = enemy.enemyPosition.z;

				enemyMessage.rotationX = 0.0f;
				enemyMessage.rotationY = enemy.enemyAngleY;

				VCU_LOG(
				    "送信前の敵座標 ID={}: ({}, {}, {})",
				    enemy.enemyId,
				    enemy.enemyPosition.x,
				    enemy.enemyPosition.y,
				    enemy.enemyPosition.z);

				networkSessionManager.Send(enemyMessage);
			}
		}

		// VCU_LOG("敵更新処理送信: {}", eCount);
	}
	break;
	default:
		break;
	}
}
