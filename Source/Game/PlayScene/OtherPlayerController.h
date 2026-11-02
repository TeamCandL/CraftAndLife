#pragma once
#include <Game/Game.h>

struct OtherPlayerControllerData
{
	// 他プレイヤーのGameObject
	std::map<std::string, GameObject> nameToObjOtherPlayer{};

	// 他プレイヤーの名前UI
	std::map<std::string, GameObject> nameToObjPlayerName{};

	// 他プレイヤーの死亡UI
	std::map<std::string, GameObject> nameToObjDeathUI{};
};

struct OtherPlayerController : ObjectBase<OtherPlayerControllerData>
{
	void Start() override;
	void Update() override;

	GameObject GetFirstOtherPlayer() const;
	std::vector<GameObject> GetOtherPlayers() const;
};
