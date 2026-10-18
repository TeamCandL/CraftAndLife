#include "PlayerControllerNormal.h"
#include "../Player.h"
#include "../MainCamera.h"

void PlayerControllerNormal::Update(PlayerData& _data)
{
	MainCameraData& data = Get<MainCamera>(_data.objMainCamera).Data();
}
