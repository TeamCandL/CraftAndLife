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
			GetScene().Destroy(obj);

			// DeathUIを削除
			auto deathIt = nameToObjDeathUI.find(name);
			if (deathIt != nameToObjDeathUI.end())
			{
				GetScene().Destroy(deathIt->second);
			}

			// PlayerNameを削除
			auto nameIt = nameToObjPlayerName.find(name);
			if (nameIt != nameToObjPlayerName.end())
			{
				GetScene().Destroy(nameIt->second);
			}
		}
	}

	// =====================================
	// 空になったオブジェクトを削除
	// =====================================

	std::erase_if(
	    nameToObjOtherPlayer,
	    [](const auto& p)
	    {
		    return p.second.IsEmpty();
	    });

	std::erase_if(
	    nameToObjDeathUI,
	    [](const auto& p)
	    {
		    return p.second.IsEmpty();
	    });

	std::erase_if(
	    nameToObjPlayerName,
	    [](const auto& p)
	    {
		    return p.second.IsEmpty();
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
		    // 他プレイヤーを生成
		    // =====================================

		    if (nameToObjOtherPlayer.count(_name) == 0)
		    {
			    nameToObjOtherPlayer[_name] = GetScene().Instantiate<OtherPlayer>(_name);
		    }

		    GameObject targetPlayer = nameToObjOtherPlayer[_name];

		    // =====================================
		    // PlayerNameを生成
		    // =====================================

		    if (nameToObjPlayerName.count(_name) == 0)
		    {
			    nameToObjPlayerName[_name] = GetScene().Instantiate<PlayerName>(_name, targetPlayer);
		    }

		    // =====================================
		    // DeathUIを生成
		    // =====================================

		    if (nameToObjDeathUI.count(_name) == 0)
		    {
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