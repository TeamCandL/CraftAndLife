#include "pch\pch.h"
#include "PlayerJobControllerHealer.h"
#include "../Player.h"
#include <Game/Manager/SessionManager.h>
#include "../MainCamera.h"

void PlayerJobControllerHealer::Update(
    PlayerData& _data)
{
	MainCamera& mainCamera = Get<MainCamera>(_data.objMainCamera);

	Vec3 positionFrom    = mainCamera.GetPosition();
	Vec3 lookAtDirection = (mainCamera.GetFocusPosition() - mainCamera.GetPosition()).Normalize();

	bool isLook{ false };
	Get<SessionManager>().ForeachPlayerStatus(
	    [this, positionFrom, lookAtDirection, &isLook](
	        const std::string& _name,
	        const InSessionStatus::PlayerStatus& _status)
	    {
		    if (isLook || Get<SessionManager>().RefStatus().name == _name)
		    {
			    return;
		    }

		    bool isDown = std::get<bool>(_status.states.at("alive"));

		    if (isDown)
		    {
			    float dot = DirectX::XMVectorGetX(
			        DirectX::XMVector3Dot((_status.position - positionFrom).Normalize(), lookAtDirection));

			    if (dot > std::cosf(downLookAngleThresholdRad))
			    {
				    // 見ている判定
				    isLook = true;
				    return;
			    }
		    }
	    });

	// 見ているならダッシュ
	if (isLook)
	{
		_data.jobStatus.moveSpeed = 0.2f;
	}
	else
	{
		_data.jobStatus.moveSpeed = 0.1f;
	}
}

void PlayerJobControllerHealer::Start(
    PlayerData& _data)
{
	_data.jobStatus = {};
}
