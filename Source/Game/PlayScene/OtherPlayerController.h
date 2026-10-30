#pragma once
#include <Game/Game.h>

struct OtherPlayerControllerData
{
	std::map<std::string, GameObject> nameToObjOtherPlayer{};  ///< ほかプレイヤーのオブジェクト
};

/// @brief ほかプレイヤーオブジェクトを操作する
struct OtherPlayerController : ObjectBase<OtherPlayerControllerData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	// 仲間プレイヤーを1人取得
	GameObject GetFirstOtherPlayer() const;

	// 仲間プレイヤー全員を取得
	std::vector<GameObject> GetOtherPlayers() const;
};
