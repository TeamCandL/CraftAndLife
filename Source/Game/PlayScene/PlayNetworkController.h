#pragma once
#include <Game/Game.h>
#include "ItemId.h"

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

	//----------------------//
	//---------/SE/---------//
	//----------------------//
	// 剣でダメージを受けたときのSE
	Resource<Audio> swordDamagedSE{ "BGM_SE/To cut with a sword.mp3" };

	// 大剣でダメージを受けたときのSE
	Resource<Audio> greatswordDamagedSE{ "BGM_SE/Strike with a greatsword.mp3" };

	// ツールでダメージを受けた時のSE
	Resource<Audio> toolDamagedSE{ "BGM_SE/ToolDamage.mp3" };

	Resource<Audio> impactSE{ "BGM_SE/Impact.mp3" };

	const float CanListenDistance{ 25.0f };  ///< 聞こえる距離

	// アイテムId
	inline static const ItemId ITEM_ID_PLAYER_ATTACK_SWORD{ 22 };      ///< 剣攻撃のアイテムId
	inline static const ItemId ITEM_ID_PLAYER_ATTACK_SWORD_BIG{ 23 };  ///< 大剣攻撃のアイテムId
	inline static const ItemId ITEM_ID_PLAYER_ATTACK_ARROW{ 24 };      ///< 矢攻撃のアイテムId
	inline static const ItemId ITEM_ID_PLAYER_ATTACK_TOOL{ 27 };       ///< 道具での攻撃のアイテムId
};

struct PlayNetworkController : ObjectBase<PlayNetworkControllerData>
{
	using ObjectBase<PlayNetworkControllerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
