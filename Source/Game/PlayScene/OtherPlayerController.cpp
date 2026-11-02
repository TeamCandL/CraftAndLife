#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include "UI/PlayerName.h"

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
	// --------------------------------

	for (auto& [name, obj] : nameToObjOtherPlayer)
	{
		if (!sessionManager.IsExistPlayer(name))
		{
			GetScene().Destroy(obj);
		}
	}

	for (auto& [name, obj] : nameToObjPlayerName)
	{
		if (!sessionManager.IsExistPlayer(name))
		{
			GetScene().Destroy(obj);
		}
	}

	// 空になったGameObjectをmapから削除
	std::erase_if(
	    nameToObjOtherPlayer,
	    [](const auto& nameAndObj)
	    {
		    return nameAndObj.second.IsEmpty();
	    });

	std::erase_if(
	    nameToObjPlayerName,
	    [](const auto& nameAndObj)
	    {
		    return nameAndObj.second.IsEmpty();
	    });

	// --------------------------------
	// 参加しているプレイヤーを確認
	// --------------------------------

	sessionManager.ForeachPlayerStatus(
	    [this, &sessionManager](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    // 自分自身は無視
		    if (sessionManager.RefStatus().name == _name)
		    {
			    return;
		    }

		    // --------------------------------
		    // OtherPlayerを生成
		    // --------------------------------

		    if (nameToObjOtherPlayer.count(_name) == 0)
		    {
			    nameToObjOtherPlayer[_name] = GetScene().Instantiate<OtherPlayer>(_name);
		    }

		    // --------------------------------
		    // PlayerName2を生成
		    // --------------------------------

		    if (nameToObjPlayerName.count(_name) == 0)
		    {
			    nameToObjPlayerName[_name] = GetScene().Instantiate<PlayerName>(_name, nameToObjOtherPlayer[_name]);
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
