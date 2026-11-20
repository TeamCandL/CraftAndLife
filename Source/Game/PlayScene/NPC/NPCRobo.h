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
	Resource<Model> model{};                         ///< モデル
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };  ///< シェーダ

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidBody{};      ///< 剛体
	Component<StaticOBB> obb{};            ///< 当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< 当たり判定描画用
	Component<Name> name{ "NPC-Tanaka" };  ///< 名前

	PlayerAnimType animIndex{};  ///< 現在のアニメーションタイプ

	int32_t hp{ 100 };            ///< hp
	float animPlaySpeed{ 1.0f };  ///< アニメーション再生速度

	GameObject objAILookAt{};  ///< 見るターゲット
	Vec2 posAiTarget{};        ///< 移動先
	ItemId itemIdWant{};       ///< 欲しいアイテムId

	using AIResult = int32_t;
	enum AIResult_ : AIResult
	{
		AIResult_Next,
		AIResult_Stop,
	};

	struct AINode
	{
		AINode()                                          = default;
		virtual ~AINode()                                 = default;
		virtual AIResult Update()                         = 0;
		virtual std::string GetNodeDesc(uint32_t _indent) = 0;

		std::string name{};
	};

	struct AINodeSequence : AINode
	{
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
			for (auto& pNode : nodes)
			{
				AIResult result = pNode->Update();
				if (result != AIResult_Next)
				{
					return result;
				}
			}
			return AIResult_Next;
		}
		std::vector<std::shared_ptr<AINode>> nodes{};
	};
	struct AINodeAction : AINode
	{
		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			return std::format("{}{}!\n", MakeIndent(_indent), name);
		}
		AIResult Update() override
		{
			return action();
		}
		std::function<AIResult()> action{};
	};
	struct AINodeCondition : AINode
	{
		std::string GetNodeDesc(
		    uint32_t _indent) override
		{
			return std::format("{}{}?\n", MakeIndent(_indent), name);
		}
		AIResult Update() override
		{
			return expression() ? AIResult_Stop : AIResult_Next;
		}
		std::function<bool()> expression{};  // 式
	};

	struct AINodeSwitch : AINode
	{
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
				if (pCondition->Update() == AIResult_Stop)
				{
					return pNode->Update();
				}
			}
		}

		std::vector<std::pair<std::shared_ptr<AINodeCondition>, std::shared_ptr<AINode>>> nodes{};
	};

	AINodeSequence rootSequenceNode{};  ///< 最も親のノード
};

struct NPCRobo : ObjectBase<NPCRoboData>
{
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
