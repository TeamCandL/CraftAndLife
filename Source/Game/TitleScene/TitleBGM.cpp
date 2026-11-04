#include "pch\pch.h"
#include "TitleBGM.h"
#include <Game/Manager/AudioManager.h>

void TitleBGM::Start()
{
	Get<AudioManager>().SetBGM(Data().titleBGM);
}