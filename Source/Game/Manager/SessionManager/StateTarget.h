#pragma once
#include <string>
#include <utility>
#include <variant>

struct StateTarget
{
	enum class Type
	{
		Self,
		Player,
		Enemy,
	};

	Type type{};

	std::variant<int32_t, std::string> player{};  /// プレイヤー名またはID (typeがPlayerの場合のみ有効)
	int enemyId{};                                ///< 敵ID (typeがEnemyの場合のみ有効)

	// @brief 自分自身を対象とするStateTargetを作成
	static StateTarget Self()
	{
		return StateTarget{ Type::Self };
	}

	// @brief 指定したプレイヤー名を対象とするStateTargetを作成
	static StateTarget Player(
	    const std::string& _playerName)
	{
		return StateTarget{ Type::Player, _playerName };
	}

	static StateTarget Player(
	    int32_t _id)
	{
		return { Type::Player, _id };
	}

	// @brief 指定した敵IDを対象とするStateTargetを作成
	static StateTarget Enemy(
	    int _enemyId)
	{
		return StateTarget{ Type::Enemy, {}, _enemyId };
	}
};