#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include "UI/PlayerName.h"
#include "UI/DeathUI.h"

#include <Game/Manager/SessionManager.h>

void OtherPlayerController::Start() {}

void OtherPlayerController::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	if (!sessionManager.HasAllPlayersStatus())
	{
		return;
	}

	// =====================================
	// 退出したプレイヤーを削除
	// =====================================

	for (auto& [name, obj] : nameToObjOtherPlayer)
	{
		if (!sessionManager.IsExistPlayer(name))
		{
			// 他プレイヤーを削除
			GetScene().Destroy(obj);

			// 対応するDeathUIも削除
			auto deathIt = nameToObjDeathUI.find(name);

			if (deathIt != nameToObjDeathUI.end())
			{
				GetScene().Destroy(deathIt->second);
			}
		}
	}

	// =====================================
	// 空になったオブジェクトをマップから削除
	// =====================================

	std::erase_if(
	    nameToObjOtherPlayer,
	    [](const auto& nameAndObj)
	    {
		    return nameAndObj.second.IsEmpty();
	    });

	std::erase_if(
	    nameToObjDeathUI,
	    [](const auto& nameAndObj)
	    {
		    return nameAndObj.second.IsEmpty();
	    });

	// =====================================
	// 他プレイヤーを確認
	// =====================================

	sessionManager.ForeachPlayerStatus(
	    [this, &sessionManager](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    // 自分自身は無視
		    if (sessionManager.RefStatus().name == _name)
		    {
			    return;
		    }

		    // =====================================
		    // 他プレイヤーがまだ生成されていない場合
		    // =====================================

		    if (nameToObjOtherPlayer.count(_name) == 0)
		    {
			    // 他プレイヤーを生成
			    nameToObjOtherPlayer[_name] = GetScene().Instantiate<OtherPlayer>(_name);
		    }

		    // =====================================
		    // DeathUIがまだ生成されていない場合
		    // =====================================

		    if (nameToObjDeathUI.count(_name) == 0)
		    {
			    // 対象となる他プレイヤーを取得
			    GameObject targetPlayer = nameToObjOtherPlayer[_name];

			    // DeathUIを生成
			    nameToObjDeathUI[_name] = GetScene().Instantiate<DeathUI>(targetPlayer);
		    }
	    });
}

GameObject OtherPlayerController::GetFirstOtherPlayer() const
{
	if (nameToObjOtherPlayer.empty())
	{
		return {};
	}

	return nameToObjOtherPlayer.begin()->second;
}

std::vector<GameObject> OtherPlayerController::GetOtherPlayers() const
{
	std::vector<GameObject> players{};

	for (const auto& [name, obj] : nameToObjOtherPlayer)
	{
		if (!obj.IsEmpty())
		{
			players.push_back(obj);
		}
	}

	return players;
}
