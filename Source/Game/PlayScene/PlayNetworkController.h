#pragma once
#include <Game/Game.h>

struct PlayNetworkControllerData
{
	PlayNetworkControllerData(
	    const GameObject _objPlayer,
	    const GameObject _objCamera,
	    const GameObject _objNetworkActorServer) :
	    objPlayer{ _objPlayer },
	    objCamera{ _objCamera },
	    objNetworkActorServer{ _objNetworkActorServer }
	{
	}

	GameObject objPlayer{};              ///< プレイヤーのオブジェクト
	GameObject objCamera{};              ///< メインカメラ
	GameObject objNetworkActorServer{};  ///< 敵のオブジェクト
};

struct PlayNetworkController : ObjectBase<PlayNetworkControllerData>
{
	using ObjectBase<PlayNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
