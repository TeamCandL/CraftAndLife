#include "pch\pch.h"
#include "PlaySceneUIController.h"
#include "UI/TaskBackground.h"
#include "UI/BossInfo.h"
#include "UI/HotBar.h"
#include "UI/MiniMap.h"
#include "UI/OtherInfo.h"
#include "UI/SelfInfo.h"
#include "UI/MyGage.h"
#include "UI/Gage1.h"
#include "UI/Gage2.h"
#include "UI/Gage3.h"
#include "UI/Gage4.h"
#include "UI/MyName.h"
#include "UI/Name1.h"

void PlaySceneUIController::Start()
{
	objTaskBackground = GetScene().Instantiate<TaskBackground>();
	objBossInfo       = GetScene().Instantiate<BossInfo>();
	objHotBar         = GetScene().Instantiate<HotBar>();
	objMiniMap        = GetScene().Instantiate<MiniMap>();
	objOtherInfo      = GetScene().Instantiate<OtherInfo>();
	objSelfInfo       = GetScene().Instantiate<SelfInfo>();
	objMyGage         = GetScene().Instantiate<MyGage>(objPlayer);
	objGage1          = GetScene().Instantiate<Gage1>();
	objGage2          = GetScene().Instantiate<Gage2>();
	objGage3          = GetScene().Instantiate<Gage3>();
	objGage4          = GetScene().Instantiate<Gage4>();
	objMyName         = GetScene().Instantiate<MyName>();
	objName1          = GetScene().Instantiate<Name1>();
}
