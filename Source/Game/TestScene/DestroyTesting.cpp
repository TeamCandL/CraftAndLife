#include "DestroyTesting.h"
#include "LifeChecker.h"

void DestroyTesting::Start()
{
	for (int i = 0; i < 3; i++)
	{
		InstantiateObj();
	}
}

void DestroyTesting::Update()
{
	if (Get<InputSystem>().GetKeyDown(KeyCode::G))
	{
		for (int i = 0; i < 3; i++)
		{
			InstantiateObj();
		}
	}

	if (Get<InputSystem>().GetKeyDown(KeyCode::D))
	{
		for (const auto gameObject : gameObjects)
		{
			Get<LifeCheckerObj>(gameObject).DestroyMe();
		}
		gameObjects.clear();
	}
}

void DestroyTesting::InstantiateObj()
{
	gameObjects.push_back(
		GetScene().Instantiate<LifeCheckerObj>(
			std::format("name({})", nameCounter++)));
}
