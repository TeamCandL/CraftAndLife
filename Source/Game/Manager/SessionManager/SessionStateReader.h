#pragma once
#include "StateTarget.h"
#include <string_view>

class SessionManager;

struct SessionStateReader
{
	explicit SessionStateReader(const SessionManager& _sessionManager);

	template <typename T>
	bool TryGetState(const StateTarget& _target, const std::string_view _stateName, T& _outValue) const;

  private:
	const SessionManager& sessionManager_;
};

#include "SessionStateReader.inl"