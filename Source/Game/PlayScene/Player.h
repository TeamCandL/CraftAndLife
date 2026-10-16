#pragma once
#include <Game/Game.h>
#include "Player/IPlayerController.h"

/// @brief プレイヤーのデータ
struct PlayerData
{
	PlayerData(const GameObject _objCamera) :
		objCamera{ _objCamera }
	{
	}

	Resource<Shader> shaderFront  ///< モデル描画用シェーダー(表面)
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};
	Resource<Shader> shaderBack  ///< モデル描画用シェーダー(裏面)
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };  ///< シェーダ
	Resource<Model> model{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-female-a.fbx" };  ///< モデル

	Component<Transform> transform{};  ///< 座標系
	Component<RigidBody> rigidBody{};  ///< 剛体
	Component<DynamicOBB> obb{};       ///< OBB当たり判定

	GameObject objCamera{};            ///< カメラオブジェクト
	GameObject objPlayerController{};  ///< プレイヤー操作するやつ

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	//-----------------------------------------//
	//パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t hp{};
	int32_t damage{};
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

	void AttackCollider();

	void Damaged(int32_t damage);
};
