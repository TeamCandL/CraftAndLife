#include "ComponentPools.h"
#include "../ComponentRegistry.h"
#include "ComponentPool.h"

namespace cha
{
	template <typename ComponentT>
	inline void ComponentPools::Register(
	    const size_t _registryIndex,
	    ComponentT* _pComponent)
	{
		const size_t COMPONENT_INDEX{ static_cast<size_t>(ComponentRegistry::GetComponentIndex<ComponentT>()) };

		if (COMPONENT_INDEX >= pool_.size())
		{
			pool_.resize(COMPONENT_INDEX + 1);
		}

		if (!pool_[COMPONENT_INDEX])
		{
			pool_[COMPONENT_INDEX] = std::make_shared<ComponentPool<ComponentT>>();
		}

		auto spComponentPool = std::static_pointer_cast<ComponentPool<ComponentT>>(pool_[COMPONENT_INDEX]);

		spComponentPool->Set(_registryIndex, _pComponent);
	}

	template <typename ComponentT>
	inline ComponentT& ComponentPools::At(
	    const size_t _objectIndex)
	{
		const size_t COMPONENT_INDEX{ static_cast<size_t>(ComponentRegistry::GetComponentIndex<ComponentT>()) };

		if (COMPONENT_INDEX >= pool_.size())
		{
			pool_.resize(COMPONENT_INDEX + 1);
		}

		if (!pool_[COMPONENT_INDEX])
		{
			pool_[COMPONENT_INDEX] = std::make_shared<ComponentPool<ComponentT>>();
		}

		auto spComponentPool = std::static_pointer_cast<ComponentPool<ComponentT>>(pool_[COMPONENT_INDEX]);

		return spComponentPool->At(_objectIndex);
	}
}  // namespace cha
