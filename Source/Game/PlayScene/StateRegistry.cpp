#include "StateRegistry.h"

namespace
{
	StateValue ReadValue(
	    const RegisteredState& state)
	{
		return std::visit(
		    [](auto ptr) -> StateValue
		    {
			    return *ptr;
		    },
		    state.value);
	}
}  // namespace

void StateRegistry::Register(
    std::initializer_list<StateEntry> entries)
{
	for (const auto& entry : entries)
	{
		std::visit(
		    [this, &entry](auto ptr)
		    {
			    Register(entry.name, *ptr);
		    },
		    entry.value);
	}
}

std::vector<StateChange> StateRegistry::GetChanges() const
{
	std::vector<StateChange> changes;

	for (const auto& state : states)
	{
		const StateValue currentValue = ReadValue(state);

		if (currentValue == state.previousValue)
		{
			continue;
		}

		changes.push_back({ state.name, currentValue });
	}

	return changes;
}

void StateRegistry::CommitChanges(
    const std::vector<StateChange>& changes)
{
	for (const auto& change : changes)
	{
		for (auto& state : states)
		{
			if (state.name == change.name)
			{
				if (state.previousValue.index() == change.value.index())
				{
					state.previousValue = change.value;
				}

				break;
			}
		}
	}
}

std::vector<StateChange> StateRegistry::GetAllStates() const
{
	std::vector<StateChange> result;

	for (const auto& state : states)
	{
		result.push_back({ state.name, ReadValue(state) });
	}

	return result;
}

std::vector<StateChange> StateRegistry::GetStates(
    bool fullSync) const
{
	if (fullSync)
	{
		return GetAllStates();
	}

	return GetChanges();
}

void StateRegistry::Unregister(
    const std::string& name)
{
	for (auto it = states.begin(); it != states.end(); ++it)
	{
		if (it->name == name)
		{
			states.erase(it);
			return;
		}
	}
}

void StateRegistry::Clear()
{
	states.clear();
}