#include "pch\pch.h"
#include "PlayerJobControllerFighter.h"
#include "../Player.h"

void PlayerJobControllerFighter::Update(
    PlayerData& _data)
{
}

void PlayerJobControllerFighter::Start(
    PlayerData& _data)
{
	_data.jobStatus = PlayerData::JobStatus{};
}
