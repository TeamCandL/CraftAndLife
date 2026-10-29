#include "PlayNetworkController.h"
#include "../TitleScene/TitleScene.h"
#include "IMainCamera.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdatePlayer.h>
#include <Game/Network/Protocols/UpdatedPlayers.h>
#include <Game/Network/Protocols/UpdateEnemy.h>
#include <Game/Network/Protocols/UpdatedEnemies.h>
#include <Game/Network/Protocols/UpdatedEnemyStates.h>
#include <Game/Network/Protocols/UpdateDayTime.h>
#include <Game/Network/Protocols/UpdatedDayTime.h>
#include <Game/PlayScene/Enemy.h>
#include <Game/PlayScene/EnemyController.h>
#include "NetworkActorServer.h"
#include <unordered_set>
#include <Game/PlayScene/DayController.h>

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
			    case ProtocolType::UpdatedEnemyStates:
			    {
				    VCU_LOG("UpdatedEnemyStatesを受信！！");

				    Protocol::UpdatedEnemyStates enemyStateMessage{};
				    converter.ToMessage(_event.data, enemyStateMessage);

				    // 敵IDと値の数が一致しているか確認
				    const auto count = enemyStateMessage.enemyStateIds.Size();

				    if (count != enemyStateMessage.values.Size())
				    {
					    VCU_LOG("敵ステートの配列サイズが一致しません");
					    break;
				    }

				    // ステート名は全敵共通
				    const std::string& stateName = enemyStateMessage.stateName;

				    for (size_t i = 0; i < count; ++i)
				    {
					    const int enemyId = enemyStateMessage.enemyStateIds[i];
					    const bool value  = enemyStateMessage.values[i];

					    VCU_LOG("敵ステート受信 ID={}, 名前={}, 値={}", enemyId, stateName, value);

					    Get<SessionManager>().UpdateEnemyState(enemyId, stateName, value);
				    }

				    break;
			    }
			    case ProtocolType::UpdatedDayTime:
			    {
				    Protocol::UpdatedDayTime dayTimeMessage{};
				    converter.ToMessage(_event.data, dayTimeMessage);
				    Get<SessionManager>().UpdateDayTime(dayTimeMessage.currentTimeSec);

				    break;
			    }
			    default:
				    VCU_LOG("想定外の謎プロトコルを取得:{}", static_cast<int>(protocolType));
				    break;
			    }
		    });

		// プレイヤーの情報更新
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

		// 敵情報更新
		NetworkActorServer& actorServer = Get<NetworkActorServer>(objNetworkActorServer);

		if (actorServer.IsHost() && actorServer.IsInitialized())
		{
			GameObject objEnemyController = actorServer.GetEnemyController();
			auto enemyStatuses            = Get<EnemyController>(objEnemyController).GetAllEnemyStatus();

			// 送信済みの敵IDを記録する
			std::unordered_set<int> enemyIds;

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

			/// 敵ステート送信
			const bool fullSync = actorServer.IsFullSync();

			// 更新履歴を確定するため、変更可能な参照で取得
			auto& registries = actorServer.GetEnemyStateRegistries();

			struct StatePacket
			{
				std::vector<int32_t> enemyIds;
				std::vector<bool> values;
			};

			// ステート名ごとに送信データをまとめる
			std::unordered_map<std::string, StatePacket> statePackets;

			for (size_t enemyId = 0; enemyId < registries.size(); ++enemyId)
			{
				if (!registries[enemyId].has_value())
					continue;

				const auto changes = registries[enemyId]->GetStates(fullSync);

				for (const auto& change : changes)
				{
					auto& packet = statePackets[change.name];

					packet.enemyIds.push_back(static_cast<int32_t>(enemyId));
					packet.values.push_back(change.value);
				}
			}

			// ステート名ごとに送信
			for (const auto& [stateName, packet] : statePackets)
			{
				Protocol::UpdatedEnemyStates message{};

				message.stateName  = stateName;
				message.isFullSync = fullSync;

				message.enemyStateIds = SizedBuffer<int32_t>(packet.enemyIds.begin(), packet.enemyIds.end());

				// vector<bool> は特殊化されているので、
				// bool を1要素ずつ SizedBuffer に代入する
				message.values.Resize(packet.values.size());

				for (size_t i = 0; i < packet.values.size(); ++i)
				{
					message.values[i] = packet.values[i];
				}

				networkSessionManager.Send(message);

				VCU_LOG("敵ステート送信 名前={}, 件数={}, 全同期={}", stateName, packet.enemyIds.size(), fullSync);
			}

			// 送信済みのステート変更を確定
			for (auto& registry : registries)
			{
				if (!registry.has_value())
					continue;

				const auto changes = registry->GetStates(fullSync);
				registry->CommitChanges(changes);
			}

			if (fullSync)
			{
				actorServer.ResetFullSync();
			}
		}

		// 昼夜サイクルの同期
		// DayControllerを取得
		DayController& dayController = Get<DayController>(objDayController);

		// ホストだけ時刻を送信
		if (actorServer.IsHost())
		{
			Protocol::UpdateDayTime dayTimeMessage{};

			dayTimeMessage.currentTimeSec = dayController.GetCurrentTimeSec();

			networkSessionManager.Send(dayTimeMessage);

			VCU_LOG("昼夜時刻送信: {}", dayTimeMessage.currentTimeSec);
		}

		// VCU_LOG("敵更新処理送信: {}", eCount);
	}
	break;
	default:
		break;
	}
}
