#pragma once
#include "Player/IPlayerController.h"
#include <Game/Game.h>

/// @brief プレイヤーのデータ
struct PlayerData
{
	/// @brief コンストラクタ
	/// @param _objCamera カメラのオブジェクト
	PlayerData(const GameObject _objCamera) : objCamera{_objCamera}
	{
	}

	Resource<Shader> shader{"Shader/Unlit.hlsl"};                                                         ///< シェーダ
	Resource<Model> model{"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-a.fbx"}; ///< モデル

	Component<Transform> transform{}; ///< 座標系
	Component<RigidBody> rigidBody{}; ///< 剛体
	Component<DynamicOBB> obb{};      ///< OBB当たり判定

	GameObject objCamera{};           ///< カメラオブジェクト
	GameObject objPlayerController{}; ///< プレイヤー操作するやつ

	float angleX{};
	float angleY{};

	int animIndex{}; ///< アニメーションのインデクス
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>
{
	using ObjectBase<PlayerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;
};
