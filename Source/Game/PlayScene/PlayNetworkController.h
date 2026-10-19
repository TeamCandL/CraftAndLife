#pragma once
#include <Game/Game.h>

struct PlayNetworkControllerData
{
	PlayNetworkControllerData(const GameObject _objPlayer, const GameObject _objCamera) :
	    objPlayer{_objPlayer},
	    objCamera{_objCamera}
	{
	}

	GameObject objPlayer{}; ///< プレイヤーのオブジェクト
	GameObject objCamera{}; ///< メインカメラ
};

struct PlayNetworkController : ObjectBase<PlayNetworkControllerData>
{
	using ObjectBase<PlayNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
