#include "EnemyStateRegistry.h"

void EnemyStateRegistry::Register(
    const std::string& name,
    const bool& value)
{
	// 同じ名前が登録されている場合は更新する
	for (auto& state : states)
	{
		if (state.name == name)
		{
			state.value         = &value;
			state.previousValue = value;
			return;
		}
	}

	// 新規登録
	states.push_back({ name, &value, value });
}

void EnemyStateRegistry::Register(
    std::initializer_list<EnemyStateEntry> entries)
{
	for (const auto& entry : entries)
	{
		Register(entry.name, entry.value);
	}
}

std::vector<EnemyStateChange> EnemyStateRegistry::GetChanges() const
{
	std::vector<EnemyStateChange> changes;

	for (const auto& state : states)
	{
		if (state.value == nullptr)
		{
			continue;
		}

		const bool currentValue = *state.value;

		if (currentValue == state.previousValue)
		{
			continue;
		}

		changes.push_back({ state.name, currentValue });
	}

	return changes;
}

void EnemyStateRegistry::CommitChanges(
    const std::vector<EnemyStateChange>& changes)
{
	for (const auto& change : changes)
	{
		for (auto& state : states)
		{
			if (state.name == change.name)
			{
				state.previousValue = change.value;
				break;
			}
		}
	}
}

std::vector<EnemyStateChange> EnemyStateRegistry::GetAllStates() const
{
	std::vector<EnemyStateChange> result;

	for (const auto& state : states)
	{
		if (state.value == nullptr)
		{
			continue;
		}

		result.push_back({ state.name, *state.value });
	}

	return result;
}

std::vector<EnemyStateChange> EnemyStateRegistry::GetStates(
    bool fullSync) const
{
	if (fullSync)
	{
		return GetAllStates();
	}

	return GetChanges();
}

void EnemyStateRegistry::Unregister(
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

void EnemyStateRegistry::Clear()
{
	states.clear();
}