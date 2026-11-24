
#pragma once

#include <Game/Game.h>
#include <cstdint>
#include "ItemId.h"

/// @brief 採取できるものの種類
enum class HarvestableType : int32_t
{
	Tree,      // 通常の木
	TreeHigh,  // 背の高い木
	Rock,      // 岩
	Mushroom   // キノコ
};

/// @brief 採取可能なもののデータ
struct HarvestableData
{
	const ItemId ITEM_ID_ROCK{ 7 };       ///< 石のアイテムId
	const ItemId ITEM_ID_WOOD{ 8 };       ///< 木のアイテムId
	const ItemId ITEM_ID_Mushroom{ 14 };  ///< キノコのアイテムId

	// 木の破壊音
	Resource<Audio> treeDestructionSE{ "BGM_SE/Tree destruction.mp3" };

	// 岩の破壊音
	Resource<Audio> rockDestructionSE{ "BGM_SE/Rock destruction.mp3" };

	// 木が破壊された瞬間のSE
	Resource<Audio> brokenTreeSE{ "BGM_SE/Broken tree.mp3" };

	// 岩が破壊された瞬間のSE
	Resource<Audio> brokenRockSE{ "BGM_SE/Broken rock.mp3" };

	// モデルの種類
	HarvestableType type{ HarvestableType::Tree };

	Component<StaticOBB> obb{};  ///< OBB当たり判定

	// 3Dモデル
	Resource<Model> model{};

	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	// モデル描画用シェーダー
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };

	// ワールド座標・回転・大きさ
	Component<Transform> transform{};

	Component<Name> name{ "ハーベスタブル" };  ///< 名前コンポーネント

	// モデルのアニメーション番号
	int32_t animIndex{ 0 };

	// 採取に必要な耐久値
	int32_t maxDurability{ 3 };
	int32_t durability{ 3 };

	uint32_t id{};  ///< このハーベスタブルオブジェクトのId

	// 採取済みかどうか
	bool isHarvested{ false };
	bool hasReportedBroken{};  ///< サーバーに報告済みか true / false

	/// @brief 配置位置と種類を指定して初期化
	HarvestableData(
	    uint32_t _id,
	    HarvestableType _type,
	    Vec3 _position,
	    float _scale = 5.0f) :
	    type{ _type },
	    id{ _id }
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

	// ワールド座標を取得
	Vec3 GetPosition() const;

	// 最大耐久値を取得
	int32_t GetMaxDurability() const;

	// 現在の耐久値を取得
	int32_t GetDurability() const;

	/// @brief ハーベスタブルオブジェクトを破壊する
	void Break();

	/// @brief ハーベスタブルオブジェクトが破壊された時の処理
	void OnBroken();
};
