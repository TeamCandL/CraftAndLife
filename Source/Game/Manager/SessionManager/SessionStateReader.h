#pragma once
#include "StateTarget.h"
#include <string_view>

class SessionManager;

struct SessionStateReader
{
	SessionStateReader(const SessionManager& _sessionManager);

	template <typename T>
	bool TryGetState(const StateTarget& _target, const std::string_view _stateName, T& _outValue) const;

	template <typename TFunc> void ForeachPlayer(TFunc&& callback) const;

  private:
	const SessionManager& sessionManager_;
};

#include "SessionStateReader.inl"