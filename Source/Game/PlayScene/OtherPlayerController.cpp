#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include "OtherPlayerHand.h"
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

	ImGui::Begin("SessionManager");

	sessionManager.ForeachPlayerStatus(
	    [](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    ImGui::Text(
		        "%s : %3.2f, %3.2f, %3.2f STATUS=%d",
		        _name.c_str(),
		        _status.position.x,
		        _status.position.y,
		        _status.position.z,
		        _status.states);
	    });

	ImGui::End();

	// --------------------------------
	// 退出したプレイヤーを削除
	// =====================================

	for (auto& [name, otherPlayerSet] : nameToObjOtherPlayer)
	{
		if (!sessionManager.IsExistPlayer(name))
		{
			GetScene().Destroy(otherPlayerSet.objBody);
			GetScene().Destroy(otherPlayerSet.objHand);

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
		    return p.second.objBody.IsEmpty();
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
			    VCU_LOG("【OtherPlayer生成】名前={}", _name);
			    nameToObjOtherPlayer[_name].objBody = GetScene().Instantiate<OtherPlayer>(_name);
			    nameToObjOtherPlayer[_name].objHand =
			        GetScene().Instantiate<OtherPlayerHand>(nameToObjOtherPlayer[_name].objBody);
		    }

		    GameObject targetPlayer = nameToObjOtherPlayer[_name].objBody;

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

	return nameToObjOtherPlayer.begin()->second.objBody;
}

std::vector<GameObject> OtherPlayerController::GetOtherPlayers() const
{
	std::vector<GameObject> players{};

	for (const auto& [name, otherPlayerSet] : nameToObjOtherPlayer)
	{
		if (!otherPlayerSet.objBody.IsEmpty())
		{
			players.push_back(otherPlayerSet.objBody);
		}
	}

	return players;
}