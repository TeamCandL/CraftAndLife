#include "pch\pch.h"
#include "PlaySceneUIController.h"
#include "UI/TaskBackground.h"
#include "UI/BossInfo.h"
#include "UI/MiniMap.h"
#include "UI/OtherInfo.h"
#include "UI/SelfInfo.h"
#include "UI/MyGauge.h"
#include "UI/Gauge1.h"
#include "UI/Gauge2.h"
#include "UI/Gauge3.h"
#include "UI/Gauge4.h"
#include "UI/MyName.h"
#include "UI/Name1.h"
#include "UI/Name2.h"
#include "UI/Name3.h"
#include "UI/Name4.h"
#include "UI/BossGaugeR.h"
#include "UI/BossGauge.h"
#include "UI/BossName.h"
#include "UI/BossHp.h"
#include "UI/CraftUI.h"
#include "UI/PlayerIcon2.h"
#include "UI/PlayerIcon3.h"
#include "UI/PlayerIcon4.h"

void PlaySceneUIController::Start()
{
	objTaskBackground = GetScene().Instantiate<TaskBackground>();
	objMiniMap        = GetScene().Instantiate<MiniMap>(objPlayer, objOtherPlayerController);
	objOtherInfo      = GetScene().Instantiate<OtherInfo>();
	objSelfInfo       = GetScene().Instantiate<SelfInfo>();
	objMyGauge        = GetScene().Instantiate<MyGauge>(objPlayer);
	objGauge1         = GetScene().Instantiate<Gauge1>();
	objGauge2         = GetScene().Instantiate<Gauge2>();
	objGauge3         = GetScene().Instantiate<Gauge3>();
	objGauge4         = GetScene().Instantiate<Gauge4>();
	objMyName         = GetScene().Instantiate<MyName>();
	objName1          = GetScene().Instantiate<Name1>();
	objName2          = GetScene().Instantiate<Name2>();
	objName3          = GetScene().Instantiate<Name3>();
	objName4          = GetScene().Instantiate<Name4>();
	objBossInfo       = GetScene().Instantiate<BossInfo>();
	objBossHp         = GetScene().Instantiate<BossHp>(objBossInfo);
	objBossName       = GetScene().Instantiate<BossName>(objBossInfo);
	objBossGaugeR     = GetScene().Instantiate<BossGaugeR>(objBossInfo);
	objBossGauge      = GetScene().Instantiate<BossGauge>(objBossInfo);
	objCraftUI        = GetScene().Instantiate<CraftUI>(objPlayer);
}
