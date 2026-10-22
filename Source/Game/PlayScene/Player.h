#pragma once
#include "IPlayer.h"
#include "Player/IPlayerController.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>

/// @brief プレイヤーのデータ
struct PlayerData
{
	PlayerData(const GameObject _objMainCamera) :
	    objMainCamera{_objMainCamera}
	{
	}

	Resource<Shader> shaderFront ///< モデル描画用シェーダー(表面)
	    {"Shader/Unlit.hlsl", RasterizerConfig{.cullMode = D3D11_CULL_FRONT}};
	Resource<Shader> shaderBack ///< モデル描画用シェーダー(裏面)
	    {"Shader/Unlit.hlsl", RasterizerConfig{.cullMode = D3D11_CULL_FRONT}};
	Resource<Shader> shader{"Shader/Unlit.hlsl"};                                                         ///< シェーダ
	Resource<Model> model{"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-a.fbx"}; ///< モデル
	Resource<Effect> sampleEffect{"Effect/Laser01.efkefc"};
	std::shared_ptr<EffectInstance> sampleEffectInstance;
	Resource<Effect> hitEffect{"Effect/Hit.efkefc"};
	std::shared_ptr<EffectInstance> hitEffectInstance;
	Component<Transform> transform{}; ///< 座標系
	Component<RigidBody> rigidBody{}; ///< 剛体
	Component<DynamicOBB> obb{};      ///< OBB当たり判定

	GameObject objMainCamera{};       ///< カメラオブジェクト
	GameObject objPlayerController{}; ///< プレイヤー操作するやつ

	PlayerRoleType role{PlayerRoleType_Max}; ///< 役職

	float angleX{};
	float angleY{};

	int animIndex{}; ///< アニメーションのインデクス

	//-----------------------------------------//
	// パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t hp{};
	int32_t damage{};
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>, IPlayer
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

	// IPlayer を介して継承されました
	Vec3 GetPosition() const override;
	float GetAngleY() const override;
};
