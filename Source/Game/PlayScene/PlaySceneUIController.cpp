#include "pch\pch.h"
#include "PlaySceneUIController.h"
#include "UI/TaskBackground.h"
#include "UI/BossInfo.h"
#include "UI/HotBar.h"
#include "UI/MiniMap.h"
#include "UI/OtherInfo.h"
#include "UI/SelfInfo.h"
#include "UI/MyGage.h"

void PlaySceneUIController::Start()
{
	objTaskBackground = GetScene().Instantiate<TaskBackground>();
	objBossInfo       = GetScene().Instantiate<BossInfo>();
	objHotBar         = GetScene().Instantiate<HotBar>();
	objMiniMap        = GetScene().Instantiate<MiniMap>();
	objOtherInfo      = GetScene().Instantiate<OtherInfo>();
	objSelfInfo       = GetScene().Instantiate<SelfInfo>();
	objMyGage         = GetScene().Instantiate<MyGage>(objPlayer);
}
