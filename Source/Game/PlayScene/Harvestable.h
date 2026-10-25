
#pragma once

#include <Game/Game.h>
#include <cstdint>

/// @brief 採取できるものの種類
enum class HarvestableType : int32_t
{
	Tree,      // 通常の木
	TreeHigh,  // 背の高い木
	Rock       // 岩
};

/// @brief 採取可能なもののデータ
struct HarvestableData
{
	// モデルの種類
	HarvestableType type{ HarvestableType::Tree };

	// 3Dモデル
	Resource<Model> model{};

	// モデル描画用シェーダー
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };

	// ワールド座標・回転・大きさ
	Component<Transform> transform{};

	// モデルのアニメーション番号
	int32_t animIndex{ 0 };

	// 採取に必要な耐久値
	int32_t maxDurability{ 3 };
	int32_t durability{ 3 };

	// 採取済みかどうか
	bool isHarvested{ false };

	/// @brief 配置位置と種類を指定して初期化
	HarvestableData(
	    HarvestableType _type,
	    Vec3 _position,
	    float _scale = 5.0f) :
	    type{ _type }
	{
		transform.position = _position;
		transform.scale    = Vec3::One() * _scale;
	}
};

/// @brief マップに配置する採取可能なもの
struct Harvestable : ObjectBase<HarvestableData>
{
	using ObjectBase<HarvestableData>::ObjectBase;

	void Start() override;
	void Update() override;
	void Draw() override;

	/// @brief 耐久値を減らす
	void TakeDamage(int32_t damage);

	/// @brief 採取済みかどうかを取得する
	bool IsHarvested() const;
};
