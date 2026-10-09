#pragma once
#include <Game/Game.h>

struct LobbyNetworkControllerData
{
	LobbyNetworkControllerData(const GameObject _objMemberList) :
		objMemberList{ _objMemberList }
	{}

	GameObject objMemberList{};
};

struct LobbyNetworkController : ObjectBase<LobbyNetworkControllerData>
{
	using ObjectBase<LobbyNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
