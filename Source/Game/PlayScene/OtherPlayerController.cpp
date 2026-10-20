#include "OtherPlayerController.h"
#include "OtherPlayer.h"
#include <Game/Manager/SessionManager.h>

void OtherPlayerController::Start()
{
}

void OtherPlayerController::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();

	for (auto& [name, obj] : nameToObjOtherPlayer)
	{
		if (!sessionManager.IsExistPlayer(name))
		{
			GetScene().Destroy(obj);
		}
	}

	std::erase_if(nameToObjOtherPlayer, [this](const auto& nameAndObj) { return nameAndObj.second.IsEmpty(); });

	sessionManager.ForeachPlayerStatus(
	    [this, &sessionManager](const std::string& _name, const InSessionStatus::PlayerStatus& _status)
	    {
		    if (sessionManager.RefStatus().name == _name)
		    {
			    // 自分自身は無視する
			    return;
		    }

		    if (nameToObjOtherPlayer.count(_name) == 0)
		    {
			    nameToObjOtherPlayer[_name] = GetScene().Instantiate<OtherPlayer>(_name);
		    }
	    });
}
