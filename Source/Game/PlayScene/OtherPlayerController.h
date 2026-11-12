#pragma once
#include <Game/Game.h>
#include "PlayerRespawnedNetworkData.h"

struct OtherPlayerControllerData
{
	struct OtherPlayerSet
	{
		GameObject objBody{};
		GameObject objHand{};
	};
	// 他プレイヤーのGameObject
	std::map<std::string, OtherPlayerSet> nameToObjOtherPlayer{};

	// 他プレイヤーの死亡UI
	std::map<std::string, GameObject> nameToObjDeathUI{};

	// 他プレイヤーの名前UI
	std::map<std::string, GameObject> nameToObjPlayerName{};

	std::vector<PlayerRespawnedData> respawn{};  ///< リスポーンデータ
};

struct OtherPlayerController : ObjectBase<OtherPlayerControllerData>
{
	void Start() override;
	void Update() override;

	GameObject GetFirstOtherPlayer() const;
	std::vector<GameObject> GetOtherPlayers() const;

	bool TryGetOtherPlayer(const std::string& playerName, GameObject& outPlayer) const;

	std::vector<PlayerRespawnedData> TakeRespawn()
	{
		return std::exchange(respawn, {});
	}

	void AddRespawn(
	    const std::string& _name,
	    const int32_t _hp)
	{
		respawn.emplace_back(_name, _hp);
	}
};
