#pragma once

#include "SessionStateReader.h"
#include "../SessionManager.h"

/// @brief 指定した対象からステートを取得する
/// @tparam T 取得するステートの型
/// @param _target 取得対象（自分自身・プレイヤー・敵）
/// @param _stateName 取得するステート名
/// @param _outValue 取得した値の格納先
/// @return ステートの取得に成功した場合はtrue、失敗した場合はfalse
template <typename T>
bool SessionStateReader::TryGetState(
    const StateTarget& _target,
    std::string_view _stateName,
    T& _outValue) const
{
	const InSessionStatus& status = sessionManager_.RefStatus();

	const std::string stateName{ _stateName };
	const std::unordered_map<std::string, StateValue>* pStates{};

	/// - Self: 自分自身のStateRegistryから取得
	/// - Player: 他プレイヤーの受信済みステートから取得
	/// - Enemy: 敵の受信済みステートから取得
	switch (_target.type)
	{
	case StateTarget::Type::Self:
	{
		const StateRegistry* registry = sessionManager_.GetSelfStateRegistry();

		if (!registry)
			return false;

		const auto states = registry->GetAllStates();

		auto itr = std::find_if(
		    states.begin(),
		    states.end(),
		    [&](const StateChange& state)
		    {
			    return state.name == stateName;
		    });

		if (itr == states.end())
			return false;

		const T* pValue = std::get_if<T>(&itr->value);

		if (!pValue)
			return false;

		_outValue = *pValue;
		return true;
	}

	case StateTarget::Type::Player:
	{
		std::string playerName;

		if (std::holds_alternative<int32_t>(_target.player))
		{
			// IDなら名前に変換
			if (!sessionManager_.TryGetPlayerName(std::get<int32_t>(_target.player), playerName))
				return false;
		}
		else
		{
			// 名前ならそのまま使用
			playerName = std::get<std::string>(_target.player);
		}

		auto itr = status.playersStatus.find(playerName);

		if (itr == status.playersStatus.end())
			return false;

		pStates = &itr->second.states;
		break;
	}

	case StateTarget::Type::Enemy:
	{
		auto itr = status.enemiesStatus.find(_target.enemyId);

		if (itr == status.enemiesStatus.end())
			return false;

		pStates = &itr->second.states;
		break;
	}

	default:
		return false;
	}

	if (!pStates)
		return false;

	auto stateItr = pStates->find(stateName);

	if (stateItr == pStates->end())
		return false;

	const T* pValue = std::get_if<T>(&stateItr->second);

	if (!pValue)
		return false;

	_outValue = *pValue;
	return true;
}

/// @brief 自分自身を含む全プレイヤーを順番に処理する
/// 最初に自分自身をStateTarget::Self()として処理する。
/// その後、playersStatusに登録されている他プレイヤーを
/// StateTarget::Player(playerName)として処理する
///
/// コールバックには以下の2つが渡される。
/// - playerName: プレイヤー名
/// - target: ステート取得に使用するStateTarget
template <typename TFunc>
void SessionStateReader::ForeachPlayer(
    TFunc&& callback) const
{
	const InSessionStatus& status = sessionManager_.RefStatus();

	// 自分自身
	callback(status.name, StateTarget::Self());

	// 他プレイヤー
	for (const auto& [playerName, playerStatus] : status.playersStatus)
	{
		if (playerName == status.name)
			continue;

		callback(playerName, StateTarget::Player(playerName));
	}
}