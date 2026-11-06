#pragma once
#include <Game/Game.h>

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
};

struct OtherPlayerController : ObjectBase<OtherPlayerControllerData>
{
	void Start() override;
	void Update() override;

	GameObject GetFirstOtherPlayer() const;
	std::vector<GameObject> GetOtherPlayers() const;

	bool TryGetOtherPlayer(const std::string& playerName, GameObject& outPlayer) const;
};
