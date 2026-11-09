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
			VCU_LOG("変更なし: {}", state.name);
			continue;
		}

		VCU_LOG("変更検出: {}", state.name);

		changes.push_back({ state.name, currentValue });

		if (state.name == "hp")
		{
			VCU_LOG("HP変更検出！");

			std::visit(
			    [](const auto& value)
			    {
				    VCU_LOG("現在のHP: {}", value);
			    },
			    currentValue);
		}
	}

	// 変更検出結果をログ出力
	for (const auto& change : changes)
	{
		VCU_LOG("ステート変更検出: Name={}", change.name);

		std::visit(
		    [](const auto& value)
		    {
			    using T = std::decay_t<decltype(value)>;

			    if constexpr (std::is_same_v<T, bool>)
			    {
				    VCU_LOG("Value={}", value ? "true" : "false");
			    }
			    else if constexpr (std::is_arithmetic_v<T>)
			    {
				    VCU_LOG("Value={}", value);
			    }
			    else
			    {
				    VCU_LOG("Value=<unsupported>");
			    }
		    },
		    change.value);
	}

	VCU_LOG("StateRegistry::GetChanges 終了: 変更数={}", changes.size());

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