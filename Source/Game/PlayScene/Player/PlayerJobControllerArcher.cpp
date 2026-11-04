#include "pch\pch.h"
#include "PlayerJobControllerArcher.h"
#include "../Player.h"
#include <Game/System/AbstructInputSystem.h>

void PlayerJobControllerArcher::Update(
    PlayerData& _data)
{
	const float DELTA_TIME = Get<DeltaTime>();

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();

	auto pPlayerInput = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	if (pPlayerInput->IsSubActionBegin())
	{
	}

	_data.transform.position = _data.transform.position + dodgeVelocity * DELTA_TIME;

	dodgeVelocity *= std::pow(friction, DELTA_TIME);
}

void PlayerJobControllerArcher::Start(
    PlayerData& _data)
{
	_data.jobStatus = PlayerData::JobStatus{};
}
