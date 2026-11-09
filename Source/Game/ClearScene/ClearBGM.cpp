#include "ClearBGM.h"
#include <Game/Manager/AudioManager.h>

void ClearBGM::Start()
{
	ManagerRegistry::Ref<AudioManager>().SetBGM(Data().bgm);
}