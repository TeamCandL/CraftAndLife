#pragma once
#include <Game/Game.h>

struct PlayNetworkControllerData
{
	PlayNetworkControllerData(const GameObject _objPlayer) :
		objPlayer{ _objPlayer }
	{}

	GameObject objPlayer{};  ///< プレイヤーのオブジェクト
};

struct PlayNetworkController : ObjectBase<PlayNetworkControllerData>
{
	using ObjectBase<PlayNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
