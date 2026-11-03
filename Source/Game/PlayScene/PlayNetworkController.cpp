#include "PlayNetworkController.h"
#include "../TitleScene/TitleScene.h"
#include "IMainCamera.h"
#include <Game/Manager/SessionManager.h>
#include <Game/Network/NetworkSessionManager.h>
#include <Game/Network/Protocols/UpdatePlayer.h>
#include <Game/Network/Protocols/UpdatedPlayers.h>
#include <Game/Network/Protocols/UpdatedPlayerStates.h>
#include <Game/Network/Protocols/UpdateEnemy.h>
#include <Game/Network/Protocols/UpdatedEnemies.h>
#include <Game/Network/Protocols/UpdatedEnemyStates.h>
#include <Game/Network/Protocols/UpdateDayTime.h>
#include <Game/Network/Protocols/UpdatedDayTime.h>
#include <Game/PlayScene/Player.h>
#include <Game/PlayScene/Enemy.h>
#include <Game/PlayScene/EnemyController.h>
#include <Game/PlayScene/NetworkActorServer.h>
#include <Game/PlayScene/StateValueCode.h>
#include <unordered_set>
#include <Game/PlayScene/DayController.h>

using enum NetworkSessionManager::State;

void PlayNetworkController::Start()
{
	Get<NetworkSessionManager>().SetCurrentState(Playing);

	Player& player = Get<Player>(objPlayer);

	NetworkActorServer& actorServer = Get<NetworkActorServer>(objNetworkActorServer);

	player.RegisterStates(actorServer.GetPlayerStateRegistry());
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

				    VCU_LOG("RECV ProtocolType::UpdatedPlayers:{}", message.positions.Size());
				    // VCU_LOG("Receive UpdatedPlayers:{}", message.flag);
				    break;
			    }
			    case ProtocolType::UpdatedPlayerStates:
			    {
				    Protocol::UpdatedPlayerStates playerStateMessage{};
				    converter.ToMessage(_event.data, playerStateMessage);

#if 1
				    Get<SessionManager>().UpdatePlayerStates(
				        playerStateMessage.playerId,
				        playerStateMessage.stateName,
				        playerStateMessage.values);
#else
				    const bool updated = Get<SessionManager>().UpdatePlayerStates(
				        playerStateMessage.playerId,
				        playerStateMessage.stateName,
				        playerStateMessage.values);

				    VCU_LOG(
				        "Receive UpdatedPlayerStates: PlayerID={}, StateName={}, Updated={}",
				        playerStateMessage.playerId,
				        playerStateMessage.stateName,
				        updated);
#endif

				    break;
			    }
			    case ProtocolType::UpdatedEnemies:
			    {
				    // VCU_LOG("UpdatedEnemiesを受信した！");
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
				    Protocol::UpdatedEnemyStates enemyStateMessage{};
				    converter.ToMessage(_event.data, enemyStateMessage);

#if 1
				    Get<SessionManager>().UpdateEnemyStates(
				        enemyStateMessage.enemyStateIds,
				        enemyStateMessage.stateName,
				        enemyStateMessage.values);

#else
				    const bool updated = Get<SessionManager>().UpdateEnemyStates(
				        enemyStateMessage.enemyStateIds,
				        enemyStateMessage.stateName,
				        enemyStateMessage.values);

				    VCU_LOG(
				        "Receive UpdatedEnemyStates: ID数={}, StateName={}, FullSync={}, Updated={}",
				        enemyStateMessage.enemyStateIds.Size(),
				        enemyStateMessage.stateName,
				        enemyStateMessage.isFullSync,
				        updated);
#endif

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

		// プレイヤーステート送信処理
		NetworkActorServer& actorServer = Get<NetworkActorServer>(objNetworkActorServer);

		StateRegistry& playerRegistry = actorServer.GetPlayerStateRegistry();
		const auto playerChanges      = playerRegistry.GetChanges();

		if (!playerChanges.empty())
		{
			const auto& status = Get<SessionManager>().RefStatus();

			// 自分のプレイヤーIDを取得
			auto nameIt = std::find(status.memberNames.begin(), status.memberNames.end(), status.name);

			// VCU_LOG("自分の名前: {}", status.name);

			for (size_t i = 0; i < status.memberNames.size(); ++i)
			{
				VCU_LOG(
				    "Member[{}]: Name={}, ID={}",
				    i,
				    status.memberNames[i],
				    i < status.memberId.size() ? status.memberId[i] : -1);
			}

			if (nameIt != status.memberNames.end())
			{
				const size_t index = std::distance(status.memberNames.begin(), nameIt);

				if (index < status.memberId.size())
				{
					Protocol::UpdatedPlayerStates message{};
					message.playerId = status.memberId[index];

					std::vector<uint8_t> values;

					for (const auto& change : playerChanges)
					{
						if (!message.stateName.empty())
						{
							message.stateName += '\n';
						}

						message.stateName += change.name;
						StateValueCode::Encode(change.value, values);
					}

					message.values.Resize(values.size());

					for (size_t i = 0; i < values.size(); ++i)
					{
						message.values[i] = values[i];
					}

					VCU_LOG("Send UpdatedPlayerStates: PlayerID={}, StateName={}", message.playerId, message.stateName);

					networkSessionManager.Send(message);

					// 送信後に変更を確定
					playerRegistry.CommitChanges(playerChanges);
				}
			}
		}

		// 敵情報更新

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
					// VCU_LOG("敵IDが重複しています: {}", enemy.enemyId);
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

			/// 敵ステート送信処理
			const bool fullSync = actorServer.IsFullSync();
			auto& registries    = actorServer.GetEnemyStateRegistries();

			Protocol::UpdatedEnemyStates enemyStateMessage{};
			enemyStateMessage.isFullSync = fullSync;

			std::vector<int32_t> enemyStateIds;
			std::vector<uint8_t> values;
			std::vector<int32_t> deadEnemyIds;

			std::vector<std::pair<size_t, std::vector<StateChange>>> pendingChanges;

			for (size_t enemyId = 0; enemyId < registries.size(); ++enemyId)
			{
				if (!registries[enemyId].has_value())
				{
					continue;
				}

				auto& registry     = *registries[enemyId];
				const auto changes = registry.GetStates(fullSync);

				for (const auto& change : changes)
				{
					enemyStateIds.push_back(static_cast<int32_t>(enemyId));

					// 型情報を含めてエンコード
					StateValueCode::Encode(change.value, values);

					if (!enemyStateMessage.stateName.empty())
					{
						enemyStateMessage.stateName += '\n';
					}

					enemyStateMessage.stateName += change.name;

					// 死亡ステートを確認
					if (change.name == "alive")
					{
						const bool* alive = std::get_if<bool>(&change.value);

						if (alive != nullptr && !(*alive))
						{
							deadEnemyIds.push_back(static_cast<int32_t>(enemyId));
						}
					}
				}

				if (!changes.empty())
				{
					pendingChanges.emplace_back(enemyId, changes);
				}
			}

			if (!enemyStateIds.empty())
			{
				enemyStateMessage.enemyStateIds = SizedBuffer<int32_t>(enemyStateIds.begin(), enemyStateIds.end());

				enemyStateMessage.values.Resize(values.size());

				for (size_t i = 0; i < values.size(); ++i)
				{
					enemyStateMessage.values[i] = values[i];
				}

				networkSessionManager.Send(enemyStateMessage);

				// 送信登録後に変更を確定
				for (const auto& [enemyId, changes] : pendingChanges)
				{
					if (enemyId < registries.size() && registries[enemyId].has_value())
					{
						registries[enemyId]->CommitChanges(changes);
					}
				}

				// 死亡通知を送信登録した敵を記録
				for (const int32_t enemyId : deadEnemyIds)
				{
					actorServer.MarkDeathQueued(enemyId);
				}

				VCU_LOG("敵ステート送信 件数={}, 全同期={}", enemyStateIds.size(), fullSync);

				if (fullSync)
				{
					actorServer.ResetFullSync();
				}
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

			// VCU_LOG("昼夜時刻送信: {}", dayTimeMessage.currentTimeSec);
		}

		// VCU_LOG("敵更新処理送信: {}", eCount);
	}
	break;
	default:
		break;
	}
}
