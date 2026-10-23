#pragma once
#include "IPlayer.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>

struct OtherPlayerData
{
	OtherPlayerData(
	    const std::string& _name) :
	    name{ _name }
	{
	}

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ

	Component<Name> name{};                                                ///< プレイヤー名
	Component<Transform> transform{ Vec3::Zero(), Vec3::One() * 200.0f };  ///< 座標系

	Resource<Model> aModel{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-c.fbx" };

	std::array<Resource<Model>, PlayerRoleType_Max> models{
		Resource<Model>{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx" },
		Resource<Model>{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx" },
		Resource<Model>{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx" },
		Resource<Model>{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx" },
	};

	PlayerRoleType role{ PlayerRoleType_Healer };

	int32_t animIndex{};
};

struct OtherPlayer : ObjectBase<OtherPlayerData>, IPlayer
{
	using ObjectBase<OtherPlayerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;

	// IPlayer を介して継承されました
	Vec3 GetPosition() const override;
	float GetAngleY() const override;
};
