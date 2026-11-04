#include "pch\pch.h"
#include "MyName.h"
#include <Game/Manager/SessionManager.h>

void MyName::Start()
{
	text.content = Get<SessionManager>().RefStatus().name;
}

void MyName::Update() {}
