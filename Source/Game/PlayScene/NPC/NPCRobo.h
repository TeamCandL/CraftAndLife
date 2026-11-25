#pragma once
#include <Game/Game.h>
#include "../Player/PlayerAnimType.h"
#include "../ItemId.h"

namespace
{
	/// @brief ひたすらインデント作るやつ
	/// @param _indent インデント数(幅は考慮せず)
	/// @return
	constexpr std::string MakeIndent(
	    const uint32_t _indent)
	{
		std::string s(_indent * 2, '\t');
		return s;
	}
}  // namespace

struct NPCRoboData
{
	const struct Props
	{
		const float NEARBY_DISTANCE{ 5.0f };           ///< どの距離を持って近いとするか
		const int32_t LOW_HP_VALUE{ 20 };              ///< 少ないと判断するhp量
		const float PERCEIVE_DEG{ 60.0f };             ///< 知覚可能な角度
		const Vec2Int MAP_POSITION_MAX{ 100, 100 };    ///< マップ座標最大値
		const Vec2Int MAP_POSITION_MIN{ -100, -100 };  ///< マップ座標最小値

		const ItemId ITEM_ID_MIN{ 3 };   ///< アイテムId:最小
		const ItemId ITEM_ID_ROCK{ 7 };  ///< アイテムId:石
		const ItemId ITEM_ID_WOOD{ 8 };  ///< アイテムId:木材
		const ItemId ITEM_ID_MAX{ 18 };  ///< アイテムId:最大

		const float MOVE_SPEED_PER_SEC{ 3.0f };  ///< 1秒間に進む速度
	} AI_CONFIG;                                 ///< AI用の設定

	Resource<ModelFbx> model{ "Model/kenney_mini-characters/Models/FBX format/character-robot.fbx" };     ///< モデル
	Resource<Shader> shader{ "Shader/UnlitFbx.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidBody{};      ///< 剛体
	Component<StaticOBB> obb{};            ///< 当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< 当たり判定描画用
	Component<Name> name{ "NPC-Tanaka" };  ///< 名前

	PlayerAnimType animIndex{};  ///< 現在のアニメーションタイプ

	int32_t hp{ 100 };            ///< hp
	float animPlaySpeed{ 1.0f };  ///< アニメーション再生速度

	GameObject objTarget{};  ///< ターゲット
	Vec3 posMoveTo{};        ///< 移動先
	ItemId itemIdWant{};     ///< 欲しいアイテムId

	// 参照したいゲームオブジェクト
	GameObject objAllPlayers{};             ///< プレイヤー
	GameObject objHarvestableController{};  ///< ハーベスタブルオブジェクト
	GameObject objEnemyController{};        ///< 敵コントローラー
	GameObject objDayController{};          ///< 日コントローラー

	// 追従させるUIたち
	GameObject objUIGiftRequestBox{};  ///< ギフトちょうだいメッセージボックス

	NPCRoboData(
	    const GameObject _objPlayer                = {},
	    const GameObject _objHarvestableController = {},
	    const GameObject _objEnemyController       = {},
	    const GameObject _objDayController         = {}) :
	    objAllPlayers{ _objPlayer },
	    objHarvestableController{ _objHarvestableController },
	    objEnemyController{ _objEnemyController },
	    objDayController{ _objDayController }
	{
	}

	using AIResult = int32_t;
	enum AIResult_ : AIResult
	{
		AIResult_No,
		AIResult_Yes,
	};

	struct AINode
	{
		AINode()                                          = default;
		virtual ~AINode()                                 = default;
		virtual AIResult Update()                         = 0;
		virtual std::string GetNodeDesc(uint32_t _indent) = 0;
	};

	struct AINodeSequence : AINode
	{
		AINodeSequence(
		    const std::string& _name,
		    std::vector<std::shared_ptr<AINode>>&& _nodes) :
		    name{ _name },
		    nodes{ std::move(_nodes) }
		{
		}

		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			std::stringstream ss{};
			for (auto& pNode : nodes)
			{
				ss << pNode->GetNodeDesc(_indent + 1) << ",\n";
			}
			return std::format("{}{}[\n{}]\n", MakeIndent(_indent), name, ss.str());
		}
		AIResult Update() override
		{
			AIResult result{};
			for (auto& pNode : nodes)
			{
				result = pNode->Update();
			}
			return result;
		}
		std::string name{};
		std::vector<std::shared_ptr<AINode>> nodes{};
	};
	struct AINodeAction : AINode
	{
		template <typename Func>
		AINodeAction(
		    const std::string& _name,
		    Func&& _func) :
		    name{ _name },
		    action{ std::forward<Func>(_func) }
		{
		}

		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			return std::format("{}{}!\n", MakeIndent(_indent), name);
		}
		AIResult Update() override
		{
			return action();
		}
		std::string name{};
		std::function<AIResult()> action{};
	};
	struct AINodeCondition : AINode
	{
		template <typename Func>
		AINodeCondition(
		    const std::string& _name,
		    Func&& _func) :
		    name{ _name },
		    expression{ std::forward<Func>(_func) }
		{
		}

		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			return std::format("{}{}?\n", MakeIndent(_indent), name);
		}
		AIResult Update() override
		{
			return expression() ? AIResult_Yes : AIResult_No;
		}
		std::string name{};
		std::function<bool()> expression{};  // 式
	};

	struct AINodeSwitch : AINode
	{
		AINodeSwitch(
		    const std::string& _name,
		    std::vector<std::pair<std::shared_ptr<AINode>, std::shared_ptr<AINode>>>&& _nodes) :
		    name{ _name },
		    nodes{ std::move(_nodes) }
		{
		}

		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			std::stringstream ss{};
			for (auto& [pCondition, pNode] : nodes)
			{
				ss << pCondition->GetNodeDesc(_indent + 1) << "->" << pNode->GetNodeDesc(0) << ",\n";
			}
			return std::format("{}{}{{\n{}}}", MakeIndent(_indent), name, ss.str());
		}
		AIResult Update() override
		{
			for (auto& [pCondition, pNode] : nodes)
			{
				if (pCondition->Update() == AIResult_Yes)
				{
					return pNode->Update();
				}
			}
		}

		std::string name{};
		std::vector<std::pair<std::shared_ptr<AINode>, std::shared_ptr<AINode>>> nodes{};
	};

	AINodeSequence rootSequenceNode{ "ルートノード", std::vector<std::shared_ptr<AINode>>{} };  ///< 最も親のノード
};

struct NPCRobo : ObjectBase<NPCRoboData>
{
	using ObjectBase<NPCRoboData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	/// @brief 描画処理
	void Draw() override;

  private:
	/// @brief AIの初期化
	void InitializeAI();

	/// @brief AIの更新
	void UpdateAI();
};
