#pragma once
#include <Game/Game.h>
#include "ItemId.h"

/// @brief 他プレイヤーの手のデータ
struct OtherPlayerHandData
{
	OtherPlayerHandData(
	    const GameObject _objParent)
	{
		transform.position = Vec3{ 0.003000f, 0.002000f, 0.001000f };
		transform.rotation = Quaternion::FromEuler(Vec3{ 0.681000f, 0.000000f, 0.000000f });
		transform.scale    = Vec3::One() * 0.005f;
		transform.parent   = _objParent;
	}

	std::string selfName{};            ///< 他プレイヤー自身の名前
	Component<Transform> transform{};  ///< 座標系
	ItemId haveItemId{};               ///< 持っているアイテムのId

	Resource<Shader> shader{ "Shader/Unlit.hlsl" };  ///< シェーダ
};

/// @brief 他プレイヤーの手
struct OtherPlayerHand : ObjectBase<OtherPlayerHandData>
{
	using ObjectBase<OtherPlayerHandData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;
};
