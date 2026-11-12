#include "AbstructInputSystem.h"

template <std::derived_from<IPlayerInput> T> inline void AbstructInputSystem::SetPlayerInput()
{
	pPlayerInput = std::make_shared<T>(*this);
}

template <std::derived_from<IPlayerInput> T>
inline void AbstructInputSystem::SetPlayerInput(
    std::shared_ptr<T> _pPlayerInput)
{
	pPlayerInput = _pPlayerInput;
}

template <std::derived_from<IUIInput> T> inline void AbstructInputSystem::SetUIInput()
{
	pUIInput = std::make_shared<T>(*this);
}

template <std::derived_from<IUIInput> T>
inline void AbstructInputSystem::SetUIInput(
    std::shared_ptr<T> _pUIInput)
{
	pUIInput = _pUIInput;
}
