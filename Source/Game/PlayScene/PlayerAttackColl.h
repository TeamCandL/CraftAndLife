#pragma once
#include <Game/Game.h>

struct PlayerAttackCollData
{
	PlayerAttackCollData()
	{
	}

	/*Resource<Shader> shaderFront  ///< モデル描画用シェーダー(表面)
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};
	Resource<Shader> shaderBack  ///< モデル描画用シェーダー(裏面)
	{
		"Shader/Unlit.hlsl",
		RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};*/
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };  ///< シェーダ
	Resource<Model> model{ "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx" };  ///< モデル

	Component<Transform> transform{};  ///< 座標系
	Component<RigidBody> rigidBody{};  ///< 剛体
	Component<DynamicOBB> obb{};       ///< OBB当たり判定

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	//-----------------------------------------//
	//パラメータ//-----------------------------//
	//-----------------------------------------//
	int32_t damage{};

};

/// @brief 敵の攻撃当たり判定
struct PlayerAttackColl : ObjectBase<PlayerAttackCollData>
{
	using ObjectBase<PlayerAttackCollData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// 対象へのダメージ付与
	void SendDamage(Enemy& enemy, int32_t);
};