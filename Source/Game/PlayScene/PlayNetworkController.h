#pragma once
#include <Game/Game.h>

struct PlayNetworkControllerData
{
	PlayNetworkControllerData(
	    const GameObject _objPlayer,
	    const GameObject _objCamera,
	    const GameObject _objNetworkActorServer,
	    const GameObject _objDayController,
	    const GameObject _objHarvestableController,
	    const GameObject _objOtherPlayerController,
	    const GameObject _objUIController) :
	    objPlayer{ _objPlayer },
	    objCamera{ _objCamera },
	    objNetworkActorServer{ _objNetworkActorServer },
	    objDayController{ _objDayController },
	    objHarvestableController{ _objHarvestableController },
	    objOtherPlayerController{ _objOtherPlayerController },
	    objUIController{ _objUIController }
	{
	}

	GameObject objPlayer{};                 ///< プレイヤーのオブジェクト
	GameObject objCamera{};                 ///< メインカメラ
	GameObject objNetworkActorServer{};     ///< 敵のオブジェクト
	GameObject objDayController{};          ///< 昼夜サイクルオブジェクト
	GameObject objHarvestableController{};  ///< ハーベスタブルオブジェクトコントローラー
	GameObject objOtherPlayerController{};  ///< 他プレイヤーの操作するやつのオブジェクト
	GameObject objUIController{};           ///< UIControllerのオブジェクト
};

struct PlayNetworkController : ObjectBase<PlayNetworkControllerData>
{
	using ObjectBase<PlayNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
