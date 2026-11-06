#pragma once

#include "SessionStateReader.h"
#include "../SessionManager.h"

template <typename T>
bool SessionStateReader::TryGetState(
    const StateTarget& _target,
    std::string_view _stateName,
    T& _outValue) const
{
	const InSessionStatus& status = sessionManager_.RefStatus();

	const std::string stateName{ _stateName };
	const std::unordered_map<std::string, StateValue>* pStates{};

	switch (_target.type)
	{
	case StateTarget::Type::Self:
	{
		auto itr = status.playersStatus.find(status.name);

		if (itr == status.playersStatus.end())
			return false;

		pStates = &itr->second.states;
		break;
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