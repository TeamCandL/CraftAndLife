#include "AbstructInputSystem.h"

template<typename T>
inline void AbstructInputSystem::SetPlayerInput()
{
	pPlayerInput = std::make_shared<T>(*this);
}

template<typename T>
inline void AbstructInputSystem::SetUIInput()
{
	pUIInput = std::make_shared<T>(*this);
}
