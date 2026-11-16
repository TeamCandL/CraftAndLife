#include "ClearBGM.h"
#include <Game/Manager/AudioManager.h>

void ClearBGM::Start()
{
	// ループなしで再生
	ManagerRegistry::Ref<AudioManager>().SetBGM(Data().bgm, false, Data().volume);
}