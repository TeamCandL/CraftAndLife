#include "pch\pch.h"
#include "NetworkActorServer.h"
#include "../Manager/SessionManager.h"

void NetworkActorServer::Start()
{
	// セッション情報からホストかどうかを取得する
	Data().isHost = Get<SessionManager>().RefStatus().imLobbyHost;
}

void NetworkActorServer::Update() {}
