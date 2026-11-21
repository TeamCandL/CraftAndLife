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
#include <Game/Network/Protocols/UpdateEnemyDamage.h>
#include <Game/Network/Protocols/UpdatedEnemyStates.h>
#include <Game/Network/Protocols/UpdateEntitySpawn.h>
#include <Game/Network/Protocols/UpdateEntityRemove.h>
#include <Game/Network/Protocols/UpdateDayTime.h>
#include <Game/Network/Protocols/UpdatedDayTime.h>
#include <Game/Network/Protocols/BreakHarvestable.h>
#include <Game/Network/Protocols/EntitySpawnRequest.h>
#include <Game/Network/Protocols/EntitySpawned.h>
#include <Game/Network/Protocols/EntityPickRequest.h>
#include <Game/Network/Protocols/EntityPicked.h>
#include <Game/Network/Protocols/PlayerRespawned.h>
#include <Game/Network/Protocols/TextChated.h>
#include <Game/PlayScene/Player.h>
#include <Game/PlayScene/Enemy.h>
#include <Game/PlayScene/EnemyController.h>
#include <Game/PlayScene/NetworkActorServer.h>
#include <Game/PlayScene/EnemyController.h>
#include <Game/PlayScene/StateValueCode.h>
#include <Game/PlayScene/EntitySystem.h>
#include <Game/PlayScene/IEntitySystemNetwork.h>
#include <Game/PlayScene/PlayerHand.h>
#include <Game/PlayScene/PlayerRespawnedNetworkData.h>
#include <unordered_set>
#include <Game/PlayScene/DayController.h>
#include <Game/PlayScene/HarvestableController.h>
#include "OtherPlayerController.h"
#include <Game/PlayScene/PlaySceneUIController.h>
#include <Game/PlayScene/UI/ChatTextLog.h>
#include <Game/Manager/AudioManager.h>
#include <Game/Common/EffectObject.h>

#include <vector>

using enum NetworkSessionManager::State;

void PlayNetworkController::Start()
{
	Get<NetworkSessionManager>().SetCurrentState(Playing);
}

void PlayNetworkController::Update()
{
	// static int testing{};
	// testing++;
	// if (testing % 6 != 0)
	// {
	//	return;
	// }

	NetworkSessionManager& networkSessionManager = Get<NetworkSessionManager>();

	NetworkActorServer& actorServer = Get<NetworkActorServer>(objNetworkActorServer);

	IEntitySystemNetwork& entitySystem = Get<EntitySystem>();

	// ホスト情報を更新
	entitySystem.SetHost(actorServer.IsHost());

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

				    VCU_LOG(
				        "UpdatedPlayers受信: Names={}, Positions={}, Rotations={}, HaveItemIds={}",
				        message.playerNames,
				        message.positions.Size(),
				        message.rotations.Size(),
				        message.haveItemIds.Size());

				    Get<SessionManager>().UpdatePlayers(
				        message.playerNames,
				        message.positions,
				        message.rotations,
				        message.haveItemIds,
				        message.attentionLevels);

				    break;
			    }
			    case ProtocolType::UpdatedPlayerStates:
			    {
				    Protocol::UpdatedPlayerStates playerStateMessage{};
				    converter.ToMessage(_event.data, playerStateMessage);

#if 1
				    VCU_LOG("UpdatedPlayerStates受信");
				    VCU_LOG("PlayerID: {}", playerStateMessage.playerId);
				    VCU_LOG("StateName: {}", playerStateMessage.stateName);
				    VCU_LOG("Values Size: {}", playerStateMessage.values.Size());

				    Get<SessionManager>().UpdatePlayerStates(
				        playerStateMessage.playerId,
				        playerStateMessage.stateName,
				        playerStateMessage.values);
#elif 0
				    VCU_LOG("PlayerID: {}", playerStateMessage.playerId);
				    VCU_LOG("StateName: {}", playerStateMessage.stateName);
				    VCU_LOG("Values Size: {}", playerStateMessage.values.Size());

				    for (size_t i = 0; i < playerStateMessage.values.Size(); ++i)
				    {
					    VCU_LOG("Values[{}]: {}", i, static_cast<int>(playerStateMessage.values[i]));
				    }

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

				    if (enemyMessage.enemyIds.Size() > 6)
				    {
					    VCU_LOG("わうわう～わんだほーい");
					    VCU_LOG("敵ID数: {}", enemyMessage.enemyIds.Size());
					    for (size_t i = 0; i < enemyMessage.enemyIds.Size(); ++i)
					    {
						    VCU_LOG("受信した敵 ID={}, Type={}", enemyMessage.enemyIds[i], enemyMessage.enemyTypes[i]);
					    }
				    }

				    Get<SessionManager>().UpdateEnemies(
				        enemyMessage.enemyIds,
				        enemyMessage.enemyTypes,
				        enemyMessage.positions,
				        enemyMessage.rotations);

				    break;
			    }
			    case ProtocolType::UpdateEnemyDamage:
			    {
				    Protocol::UpdateEnemyDamage message{};
				    converter.ToMessage(_event.data, message);

				    // NetworkActorServerを取得
				    NetworkActorServer& networkActorServer = Get<NetworkActorServer>(Data().objNetworkActorServer);

				    // NetworkActorServerを取得
				    NetworkActorServer& actorServer = Get<NetworkActorServer>(Data().objNetworkActorServer);

				    // ホストでなければ処理しない
				    if (!actorServer.IsHost() || !actorServer.IsInitialized())
				    {
					    break;
				    }

				    // EnemyControllerを取得
				    GameObject objEnemyController = networkActorServer.GetEnemyController();

				    // 対象の敵にダメージを与える
				    Get<EnemyController>(objEnemyController).DamagedEnemy(message.enemyId, message.damage);

				    break;
			    }
			    case ProtocolType::UpdatedEnemyStates:
			    {
				    Protocol::UpdatedEnemyStates enemyStateMessage{};
				    converter.ToMessage(_event.data, enemyStateMessage);

#if 0
				    Get<SessionManager>().UpdateEnemyStates(
				        enemyStateMessage.enemyStateIds,
				        enemyStateMessage.stateName,
				        enemyStateMessage.values);

#elif 1
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
			    case ProtocolType::UpdateEntitySpawn:
			    {
				    Protocol::UpdateEntitySpawn message{};
				    converter.ToMessage(_event.data, message);

				    Get<EntitySystem>().SpawnFromNetwork(
				        message.entityId,
				        static_cast<ItemId>(message.itemId),
				        Vec3{ message.positionX, message.positionY, message.positionZ },
				        Vec3{ message.velocityX, message.velocityY, message.velocityZ });

				    Vec3 pos  = { message.positionX, message.positionY, message.positionZ };
				    Vec3 velo = { message.velocityX, message.velocityY, message.velocityZ };
				    VCU_LOG(
				        "Receve UpdateEntitySpawn: ID={}, Pos={},{},{}, velo={},{},{}",
				        message.entityId,
				        pos.x,
				        pos.y,
				        pos.z,
				        velo.x,
				        velo.y,
				        velo.z);

				    break;
			    }
				/*case ProtocolType::UpdateEntityRemove:
				{
				    Protocol::UpdateEntityRemove message{};
				    converter.ToMessage(_event.data, message);

				    Get<EntitySystem>().RemoveFromNetwork(message.entityId);

				    VCU_LOG("エンティティ削除情報受信 ID={}", message.entityId);

				    break;
				}*/
			    case ProtocolType::UpdatedDayTime:
			    {
				    Protocol::UpdatedDayTime dayTimeMessage{};
				    converter.ToMessage(_event.data, dayTimeMessage);
				    Get<SessionManager>().UpdateDayTime(dayTimeMessage.currentTimeSec, dayTimeMessage.dayCount);
				    VCU_LOG(
				        "UpdatedDayTime受信: currentTimeSec={}, dayCount={}",
				        dayTimeMessage.currentTimeSec,
				        dayTimeMessage.dayCount);
				    break;
			    }
			    case ProtocolType::BreakHarvestable:
			    {
				    Protocol::BreakHarvestable breakHarvestableMessage{};
				    converter.ToMessage(_event.data, breakHarvestableMessage);
				    Get<HarvestableController>(objHarvestableController)
				        .BreakAt(breakHarvestableMessage.breakHarvestableId);

				    break;
			    }
			    case ProtocolType::EntitySpawned:
			    {
				    Protocol::EntitySpawned message{};
				    converter.ToMessage(_event.data, message);

				    IEntitySystemNetwork& entitySystem = Get<EntitySystem>();
				    entitySystem.SpawnFromNetwork(
				        message.entityId,
				        message.itemId,
				        Vec3{ message.positionX, message.positionY, message.positionZ },
				        Vec3{
				            message.velocityX,
				            message.velocityY,
				            message.velocityZ,
				        });
				    break;
			    }
			    case ProtocolType::EntityPicked:
			    {
				    Protocol::EntityPicked message{};
				    converter.ToMessage(_event.data, message);

				    Vec3 position = Vec3{ message.positionX, message.positionY, message.positionZ };

				    if (Get<SessionManager>().RefStatus().name == message.pickerName)
				    {
					    VCU_LOG("自分が拾った！！");
					    Get<Player>(objPlayer).GiveItem(message.itemId, position);
				    }
				    else
					    VCU_LOG("他人が拾った！！");

				    // エンティティ拾ったときの座標が音聞こえる範囲なら
				    if (true)  // (Get<Transform>(objPlayer).position - position).Length() <= CanListenDistance
				    {
					    switch (message.itemId)
					    {
					    case ITEM_ID_PLAYER_ATTACK_SWORD:
						    // 剣の音を鳴らす（音量50%）
						    Get<AudioManager>().Play(swordDamagedSE, 0.5f);
						    break;
					    case ITEM_ID_PLAYER_ATTACK_SWORD_BIG:
						    // 大剣の音を鳴らす（音量50%）
						    Get<AudioManager>().Play(greatswordDamagedSE, 0.5f);
						    break;
					    case ITEM_ID_PLAYER_ATTACK_ARROW:
						    // 矢の着弾の音を鳴らす
						    Get<AudioManager>().Play(impactSE);
						    break;
					    default:
						    // それ以外は音なし 他の人がアイテム拾ったときの音とかもできるよ
						    break;
					    }

					    // ヒットエフェクト
					    switch (message.itemId)
					    {
					    case ITEM_ID_PLAYER_ATTACK_SWORD:
					    case ITEM_ID_PLAYER_ATTACK_SWORD_BIG:
					    case ITEM_ID_PLAYER_ATTACK_ARROW:
						    GetScene().Instantiate<OnceEffectObject>("Effect/Hit.efkefc", position);
						    break;
					    default:
						    // それ以外はエフェクトなし
						    break;
					    }
				    }

				    IEntitySystemNetwork& entitySystem = Get<EntitySystem>();
				    entitySystem.RemoveFromNetwork(message.entityId);
				    break;
			    }
			    case ProtocolType::PlayerRespawned:
			    {
				    Protocol::PlayerRespawned message{};
				    converter.ToMessage(_event.data, message);

				    if (Get<SessionManager>().RefStatus().name == message.playerName)
				    {
					    VCU_LOG("自分が生き返った！！");
					    Get<Player>(objPlayer).Respawn(message.hp);
				    }
				    break;
			    }
			    case ProtocolType::TextChated:
			    {
				    Protocol::TextChated message{};
				    converter.ToMessage(_event.data, message);

				    PlaySceneUIController& playSceneUIController = Get<PlaySceneUIController>(objUIController);

				    GameObject objChatTextLog = playSceneUIController.Data().objChatTextLog;

				    // 自分の名前を取得
				    const std::string& localName = Get<SessionManager>().RefStatus().name;

				    // 他プレイヤーからのログか判定
				    const bool isOtherPlayer = !message.sender.empty() && message.sender != localName;

				    // ログは全員に表示、通知音は他プレイヤーの場合のみ
				    Get<ChatTextLog>(objChatTextLog).AddLog(message.content, isOtherPlayer);

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

		message.haveItemId     = Get<PlayerHand>(Get<Player>(objPlayer).Data().objPlayerHand).Data().haveItemId;
		message.attentionLevel = Get<Player>(objPlayer).Data().attentionLevel;
		VCU_LOG("送信する注目度: {}", message.attentionLevel);

		// VCU_LOG("更新処理送信: {}", counter);

		networkSessionManager.Send(message);

		// プレイヤーステート送信処理
		static bool isFirstSync       = true;
		StateRegistry& playerRegistry = actorServer.GetPlayerStateRegistry();
		std::vector<StateChange> playerChanges;

		if (isFirstSync)
		{
			// 初回同期時は全ステートを送信する
			playerChanges = playerRegistry.GetAllStates();
		}
		else
		{
			// 2回目以降は変更があったステートのみ送信する
			playerChanges = playerRegistry.GetChanges();
		}

		const auto& status = Get<SessionManager>().RefStatus();

		if (!playerChanges.empty())
		{
			Protocol::UpdatedPlayerStates stateMessage{};

			auto nameIt = std::find(status.memberNames.begin(), status.memberNames.end(), status.name);

			if (nameIt != status.memberNames.end())
			{
				const size_t index = std::distance(status.memberNames.begin(), nameIt);

				if (index < status.memberId.size())
				{
					stateMessage.playerId = status.memberId[index];

					VCU_LOG("送信するプレイヤーID: {}", stateMessage.playerId);

					std::vector<uint8_t> values;

					for (const auto& change : playerChanges)
					{
						if (!stateMessage.stateName.empty())
							stateMessage.stateName += '\n';

						stateMessage.stateName += change.name;
						StateValueCode::Encode(change.value, values);
					}

					stateMessage.values.Resize(values.size());

					for (size_t i = 0; i < values.size(); ++i)
						stateMessage.values[i] = values[i];

					VCU_LOG("送信ステート名: {}", stateMessage.stateName);

					networkSessionManager.Send(stateMessage);

					VCU_LOG("プレイヤーステート送信処理実行");

					playerRegistry.CommitChanges(playerChanges);

					if (isFirstSync)
						isFirstSync = false;
				}
				else
				{
					VCU_LOG("プレイヤーIDのインデックスが範囲外");
				}
			}
			else
			{
				VCU_LOG("自分の名前がメンバー一覧に存在しない");
			}
		}
		else
		{
			VCU_LOG("送信するプレイヤーステートがありません");
		}

		// 敵情報更新
		if (actorServer.IsHost() && actorServer.IsInitialized())
		{
			GameObject objEnemyController = actorServer.GetEnemyController();
			if (!objEnemyController.IsEmpty())
			{
				auto enemyStatuses = Get<EnemyController>(objEnemyController).GetAllEnemyStatus();

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

					Protocol::UpdateEnemy enemyMessage;
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
				// vectorからsizedBufferに変換
				/*enemyMessage.enemyIds   = SizedBuffer<int32_t>(std::move(enemyIds));
				enemyMessage.enemyTypes = SizedBuffer<int32_t>(std::move(enemyTypes));
				enemyMessage.positions  = SizedBuffer<float>(std::move(positions));
				enemyMessage.rotations  = SizedBuffer<float>(std::move(rotations));*/

				// 敵ステート送信処理
				const bool fullSync = actorServer.IsFullSync();
				auto& registries    = actorServer.GetEnemyStateRegistries();

				const auto& pendingDeaths = actorServer.GetPendingEnemyDeaths();

				Protocol::UpdatedEnemyStates enemyStateMessage{};
				enemyStateMessage.isFullSync = fullSync;

				std::vector<int32_t> enemyStateIds;
				std::vector<uint8_t> values;

				std::vector<std::pair<size_t, std::vector<StateChange>>> pendingChanges;

				VCU_LOG("敵ステート送信 FullSync={}", fullSync);
				VCU_LOG("敵ステート登録数={}", registries.size());

				// ステート名を改行区切りで追加
				auto AddStateName = [&enemyStateMessage](const std::string& name)
				{
					if (!enemyStateMessage.stateName.empty())
					{
						enemyStateMessage.stateName += '\n';
					}

					enemyStateMessage.stateName += name;
				};

				for (size_t enemyId = 0; enemyId < registries.size(); ++enemyId)
				{
					if (!registries[enemyId].has_value())
						continue;

					// 死亡済みの敵はレジストリのポインタを参照しない
					if (actorServer.IsDeathQueued(static_cast<int32_t>(enemyId)))
					{
						continue;
					}

					// 初回は全ステート、それ以降は変更分だけ取得
					auto& registry     = *registries[enemyId];
					const auto changes = registry.GetStates(fullSync);

					VCU_LOG("敵 ID={} 取得ステート数={}", enemyId, changes.size());

					for (const auto& change : changes)
					{
						VCU_LOG("送信対象 ID={} Name={}", enemyId, change.name);
						enemyStateIds.push_back(static_cast<int32_t>(enemyId));

						// 値をエンコード
						StateValueCode::Encode(change.value, values);

						AddStateName(change.name);
					}

					if (!changes.empty())
						pendingChanges.emplace_back(enemyId, changes);
				}

				// 死亡通知の送信
				for (const auto& death : pendingDeaths)
				{
					enemyStateIds.push_back(death.enemyId);
					StateValueCode::Encode(StateValue{ int32_t{ 0 } }, values);
					AddStateName("hp");

					enemyStateIds.push_back(death.enemyId);

					// 死亡通知は alive = false
					StateValueCode::Encode(StateValue{ death.alive }, values);
					AddStateName("alive");
				}

				// 送信するステートがある場合のみ送信
				if (!enemyStateIds.empty())
				{
					enemyStateMessage.enemyStateIds = SizedBuffer<int32_t>(enemyStateIds.begin(), enemyStateIds.end());

					enemyStateMessage.values.Resize(values.size());

					for (size_t i = 0; i < values.size(); ++i)
						enemyStateMessage.values[i] = values[i];

					networkSessionManager.Send(enemyStateMessage);

					// 送信後に変更を確定
					for (const auto& [enemyId, changes] : pendingChanges)
					{
						if (enemyId < registries.size() && registries[enemyId].has_value())
							registries[enemyId]->CommitChanges(changes);
					}

					// 死亡通知を送信した敵のレジストリを解除
					for (const auto& death : pendingDeaths)
					{
						actorServer.ReleaseActor(death.enemyId);
					}

					// 死亡通知キューをクリア
					if (!pendingDeaths.empty())
					{
						actorServer.ClearPendingEnemyDeaths();
					}

					// 初回同期完了
					if (fullSync)
						actorServer.ResetFullSync();
				}
			}
		}

		IEntitySystemNetwork& entitySystem = Get<EntitySystem>();

		// エンティティスポーンしたリクエストを処理
		std::vector<EntitySpawnData> entitySpawnsData = entitySystem.TakeSpawnsData();
		for (const auto& data : entitySpawnsData)
		{
			Protocol::EntitySpawnRequest message{};
			message.itemId    = data.itemId;
			message.positionX = data.position.x;
			message.positionY = data.position.y;
			message.positionZ = data.position.z;
			message.velocityX = data.velocity.x;
			message.velocityY = data.velocity.y;
			message.velocityZ = data.velocity.z;

			networkSessionManager.Send(message);
		}

		// エンティティを拾ったリクエストを処理
		std::vector<EntityPickData> entityPicksData = entitySystem.TakePicksData();
		for (const auto& data : entityPicksData)
		{
			Protocol::EntityPickRequest message{};
			message.entityId  = data.entityId;
			message.positionX = data.position.x;
			message.positionY = data.position.y;
			message.positionZ = data.position.z;

			networkSessionManager.Send(message);
		}

		OtherPlayerController& otherPlayerController          = Get<OtherPlayerController>(objOtherPlayerController);
		std::vector<PlayerRespawnedData> playerRespawnedsData = otherPlayerController.TakeRespawn();
		for (auto& data : playerRespawnedsData)
		{
			Protocol::PlayerRespawned message{};

			message.playerName = data.playerName;
			message.hp         = data.hp;

			networkSessionManager.Send(message);
		}

		PlaySceneUIController& playSceneUIController = Get<PlaySceneUIController>(objUIController);

		std::vector<std::string> requestLogs =
		    Get<ChatTextLog>(playSceneUIController.Data().objChatTextLog).TakeLogRequests();

		for (auto& log : requestLogs)
		{
			Protocol::TextChated message{};
			message.content = log;
			message.sender  = Get<SessionManager>().RefStatus().name;

			networkSessionManager.Send(message);
		}

		/*  // エンティティシステムを変更したため無視
		// エンティティ生成情報送信
		EntitySystem& entitySystem = Get<EntitySystem>();

		const auto& spawnDataList = entitySystem.GetPendingSpawnData();

		for (const EntitySpawnData& spawnData : spawnDataList)
		{
		    Protocol::UpdateEntitySpawn entityMessage{};

		    entityMessage.entityId   = spawnData.entityId;
		    entityMessage.itemId     = spawnData.itemId;
		    entityMessage.sourceType = static_cast<uint8_t>(spawnData.sourceType);
		    entityMessage.sourceId   = spawnData.sourceId;

		    entityMessage.positionX = spawnData.position.x;
		    entityMessage.positionY = spawnData.position.y;
		    entityMessage.positionZ = spawnData.position.z;

		    entityMessage.velocityX = spawnData.velocity.x;
		    entityMessage.velocityY = spawnData.velocity.y;
		    entityMessage.velocityZ = spawnData.velocity.z;

		    networkSessionManager.Send(entityMessage);
		    VCU_LOG(
		        "エンティティの情報送信: ID={},itemID={},Type={},posX={},velX={}",
		        entityMessage.entityId,
		        entityMessage.itemId,
		        entityMessage.sourceType,
		        entityMessage.positionX,
		        entityMessage.velocityX);
		}

		// 送信後に送信待ちデータをクリア
		if (!spawnDataList.empty())
		{
		    entitySystem.ClearPendingSpawnData();
		}

		// エンティティ削除情報送信
		const auto& removeDataList = entitySystem.GetPendingRemoveData();

		for (const EntityRemoveData& removeData : removeDataList)
		{
		    Protocol::UpdateEntityRemove removeMessage{};

		    removeMessage.entityId = removeData.entityId;

		    networkSessionManager.Send(removeMessage);

		    VCU_LOG("エンティティ削除情報送信 ID={}", removeMessage.entityId);
		}

		// 送信後に送信待ちデータをクリア
		if (!removeDataList.empty())
		{
		    entitySystem.ClearPendingRemoveData();
		}
		*/

		// 昼夜サイクルの同期
		// DayControllerを取得
		DayController& dayController = Get<DayController>(objDayController);

		// ホストだけ時刻を送信
		if (actorServer.IsHost())
		{
			Protocol::UpdateDayTime dayTimeMessage{};

			dayTimeMessage.currentTimeSec = dayController.GetCurrentTimeSec();
			dayTimeMessage.dayCount       = dayController.GetCurrentDay();

			VCU_LOG("昼夜時刻送信: {} 日数: {}", dayTimeMessage.currentTimeSec, dayTimeMessage.dayCount);

			networkSessionManager.Send(dayTimeMessage);

			// VCU_LOG("昼夜時刻送信: {}", dayTimeMessage.currentTimeSec);
		}

		// VCU_LOG("敵更新処理送信: {}", eCount);

		HarvestableController& harvestableController = Get<HarvestableController>(objHarvestableController);
		if (std::optional<uint32_t> foundId = harvestableController.FindToLocalBokenIds())
		{
			Protocol::BreakHarvestable breakHarvestableMessage{};
			breakHarvestableMessage.breakHarvestableId = *foundId;
			networkSessionManager.Send(breakHarvestableMessage);
		}

		networkSessionManager.Flush();
	}
	break;
	default:
		break;
	}
}