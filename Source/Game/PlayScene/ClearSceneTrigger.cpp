#include "ClearSceneTrigger.h"
#include <Game/ClearScene/ClearScene.h>
#include "DayController.h"
#include "ClearFade.h"

#include <Game/Manager/SessionManager.h>
#include "Player/HelthState.h"
#include "EnemyController.h"

namespace
{
	/// @brief 参加中の全プレイヤーが死亡しているか
	bool AreAllPlayersDead(
	    SessionManager& _sessionManager)
	{
		const InSessionStatus& STATUS = _sessionManager.RefStatus();

		// 参加者がいなければ判定しない
		if (STATUS.memberNames.empty())
		{
			return false;
		}

		// 自分自身のステートを取得
		const StateRegistry* SELF_REGISTRY = _sessionManager.GetSelfStateRegistry();

		// 自分のステートが登録されていなければ判定しない
		if (SELF_REGISTRY == nullptr)
		{
			return false;
		}

		// 自分自身のステート一覧
		const std::vector<StateChange> SELF_STATES = SELF_REGISTRY->GetAllStates();

		// 参加者全員の死亡状態を確認
		for (const std::string& playerName : STATUS.memberNames)
		{
			// 自分自身の場合
			if (playerName == STATUS.name)
			{
				auto it = std::ranges::find(SELF_STATES, std::string{ "helthState" }, &StateChange::name);

				// 自分の死亡状態が取得できなければ判定しない
				if (it == SELF_STATES.end())
				{
					return false;
				}

				const int32_t* healthState = std::get_if<int32_t>(&it->value);

				if (healthState == nullptr || *healthState != HelthState_Dead)
				{
					return false;
				}
			}
			// 他プレイヤーの場合
			else
			{
				auto playerIt = STATUS.playersStatus.find(playerName);

				// プレイヤー情報がまだ届いていない
				if (playerIt == STATUS.playersStatus.end())
				{
					return false;
				}

				const auto& STATES = playerIt->second.states;

				auto stateIt = STATES.find("helthState");

				// 死亡状態がまだ届いていない
				if (stateIt == STATES.end())
				{
					return false;
				}

				const int32_t* healthState = std::get_if<int32_t>(&stateIt->second);

				// int32_t以外、または生存中なら全員死亡ではない
				if (healthState == nullptr || *healthState != HelthState_Dead)
				{
					return false;
				}
			}
		}

		// 全員の死亡を確認できた
		return true;
	}
}  // namespace

void ClearSceneTrigger::Update()
{
	// フェードオブジェクトを取得
	ClearFade& fade = Get<ClearFade>(Data().objClearFade);

	// フェード中なら何もしない
	if (fade.Data().isFading)
	{
		return;
	}

	// Backspaceが押されたらゲームオーバー
	if (Get<InputSystem>().GetKeyDown(KeyCode::Back))
	{
		fade.StartFade(ResultType::GameOver);
		return;
	}

	// 全プレイヤーが死亡したらゲームオーバー
	SessionManager& sessionManager = Get<SessionManager>();

	if (AreAllPlayersDead(sessionManager))
	{
		fade.StartFade(ResultType::GameOver);
		return;
	}

	// Enterが押されたらクリア
	if (Get<InputSystem>().GetKeyDown(KeyCode::Enter))
	{
		fade.StartFade(ResultType::Clear);
		return;
	}

	// 現在の日数と時刻を取得
	DayController& dayController = Get<DayController>(Data().objDayController);

	const int32_t CURRENT_DAY = dayController.GetCurrentDay();

	const float CURRENT_TIME = dayController.GetCurrentTimeSec();

	// ここでクリア条件を設定する。

	bool isDragonAlive  = true;
	bool hasDragonState = false;

	if (sessionManager.RefStatus().imLobbyHost)
	{
		// ホスト側の EnemyController から取得
		NetworkActorServer& networkActorServer = Get<NetworkActorServer>(Data().objNetworkActorServer);

		GameObject objEnemyController = networkActorServer.GetEnemyController();

		if (!objEnemyController.IsEmpty())
		{
			EnemyController& enemyController = Get<EnemyController>(objEnemyController);

			isDragonAlive  = enemyController.IsExistEnemy(10);
			hasDragonState = true;
		}
	}
	else
	{
		// ゲスト側は同期されたステートから取得
		hasDragonState = sessionManager.TryGetEnemyAlive(10, isDragonAlive);
	}

	const InSessionStatus& status = sessionManager.RefStatus();
	if (CURRENT_DAY >= 3 && CURRENT_TIME >= 45.0f && status.currentTimeSec >= 45.0f)
	{
		// ドラゴン撃破でクリア
		if (hasDragonState && !isDragonAlive)
		{
			fade.StartFade(ResultType::Clear);
			return;
		}

		// 45秒過ぎた時ボスを倒せなければゲームオーバー
		if (CURRENT_TIME >= 46.0f && status.currentTimeSec >= 46.0f)
		{
			fade.StartFade(ResultType::GameOver);
			return;
		}
	}
}