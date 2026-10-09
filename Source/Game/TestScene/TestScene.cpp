
#include "TestScene.h"
#include "../Common/GameManager.h"
#include "SceneTester.h"
#include "DestroyTesting.h"

void TestScene::Start()
{
	Instantiate<GameManager>();

	Instantiate<DestroyTesting>();

	Instantiate<SceneTester>("TestScene");
}
