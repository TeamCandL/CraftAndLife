#include "pch\pch.h"
#include "PlayerJobControllerArcher.h"
#include "../Player.h"
#include <Game/System/AbstructInputSystem.h>
#include <Game/Manager/AudioManager.h>

void PlayerJobControllerArcher::Update(
    PlayerData& _data)
{
	const float DELTA_TIME = Get<DeltaTime>();

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();

	auto pPlayerInput = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	Vec2 dir2D = (bodyMove + Vec2::Down() * 1.0f).Normalize();

	Vec3 dir = DirectX::XMVector3Transform(Vec3{ dir2D.x, 0.0f, dir2D.y }, _data.transform.world);

	if (pPlayerInput->IsSubActionBegin())
	{
		// ダッシュSE
		Get<AudioManager>().Play(dashSE);

		_data.jobStatus.moveVelocity = dir * dashForce;
	}

	//_data.transform.position = _data.transform.position + _data.jobStatus.moveVelocity * DELTA_TIME;

	_data.jobStatus.moveVelocity *= std::pow(friction, DELTA_TIME);
}

void PlayerJobControllerArcher::Start(
    PlayerData& _data)
{
	_data.jobStatus = PlayerData::JobStatus{};
}