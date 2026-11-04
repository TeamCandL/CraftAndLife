#include "pch\pch.h"
#include "PlayerJobControllerTank.h"
#include "../Player.h"

void PlayerJobControllerTank::Update(
    PlayerData& _data)
{
}

void PlayerJobControllerTank::Start(
    PlayerData& _data)
{
	_data.jobStatus = PlayerData::JobStatus{
		.moveSpeed = 0.07f,
	};
}
