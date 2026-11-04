#include "pch\pch.h"
#include "TaskBackground.h"
#include <Game/Manager/SessionManager.h>

void TaskBackground::Start()
{
	text.content = "テストタスク";
}

void TaskBackground::Update()
{
	text.content = Get<SessionManager>().RefStatus().imLobbyHost ? "自分はホストだよ" : "自分はゲストだよ";
}
