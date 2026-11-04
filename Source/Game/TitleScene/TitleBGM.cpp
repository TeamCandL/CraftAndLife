#include "pch\pch.h"
#include "TitleBGM.h"
#include <Game/System/AudioSystem.h>

void TitleBGM::Start()
{
	Get<AudioSystem>().SetBGM(Data().titleBGM);
}