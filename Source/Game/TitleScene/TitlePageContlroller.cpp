#include "pch/pch.h"
#include "TitlePageContlroller.h"

// 追加の.h
#include "TitleLogo.h"
#include "TitleLogoStartGuide.h"

void TitlePageController::ChangePage(
    Page page)
{
	// 現在表示中のページを削除
	ClearPage();

	// ページを更新
	currentPage = page;

	// 指定されたページを生成
	switch (currentPage)
	{
	case Page::Title:
		CreateTitlePage();
		break;

	case Page::TeamSelect:
		CreateTeamSelectPage();
		break;

	case Page::PlayerName:
		CreatePlayerNamePage();
		break;

	case Page::Lobby:
		CreateLobbyPage();
		break;

	case Page::GameStart:
		CreateGameStartPage();
		break;

	default:
		break;
	}
}

void TitlePageController::ClearPage() {}

void TitlePageController::CreateTitlePage() {}
void TitlePageController::CreateTeamSelectPage() {}

void TitlePageController::CreatePlayerNamePage() {}

void TitlePageController::CreateLobbyPage() {}

void TitlePageController::CreateGameStartPage() {}
