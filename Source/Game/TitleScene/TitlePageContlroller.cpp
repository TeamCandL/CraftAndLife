
#include "pch/pch.h"
#include "TitlePageContlroller.h"

// 各ページのUIヘッダー
#include "TitleLogo.h"
#include "TitleLogoStartGuide.h"
#include "TeamSelect.h"
#include "TeamSelectStartGuide.h"
#include "CreatePlayerName.h"
#include "GameStart.h"
#include "GameStartGuide.h"
#include "Lobby.h"
#include "LobbyTeamName.h"
#include "LobbyPlayer.h"
#include "LobbyPlayerName.h"
#include "LobbyPlayerNameOne.h"
#include "LobbyPlayerNameTwo.h"
#include "LobbyPlayerNameTree.h"

/// @brief ページ変更
void TitlePageController::ChangePage(
    PageType page)
{
	// 現在表示されているUIを削除
	ClearPage();

	// 指定されたページのUIを生成
	switch (page)
	{
	case PageType::Title:
		CreateTitlePage();
		break;

	case PageType::TeamSelect:
		CreateTeamSelectPage();
		break;

	case PageType::PlayerName:
		CreatePlayerNamePage();
		break;

	case PageType::Lobby:
		CreateLobbyPage();
		break;

	case PageType::GameStart:
		CreateGameStartPage();
		break;

	default:
		break;
	}
}

/// @brief 現在のページのUIを削除
void TitlePageController::ClearPage()
{
	for (auto obj : data.objUIs)
	{
		GetScene().Destroy(obj);
	}

	// 管理リストを空にする
	data.objUIs.clear();
}

/// @brief タイトルページ
void TitlePageController::CreateTitlePage()
{
	data.objUIs.push_back(GetScene().Instantiate<TitleLogo>());

	// data.objUIs.push_back(GetScene().Instantiate<TitleLogoStartGuide>());
}

/// @brief チーム選択ページ
void TitlePageController::CreateTeamSelectPage()
{
	data.objUIs.push_back(GetScene().Instantiate<TeamSelect>());

	data.objUIs.push_back(GetScene().Instantiate<TeamSelectStartGuide>());
}

/// @brief プレイヤー名入力ページ
void TitlePageController::CreatePlayerNamePage()
{
	data.objUIs.push_back(GetScene().Instantiate<CreatePlayerName>());
}

/// @brief ロビーページ
void TitlePageController::CreateLobbyPage()
{
	data.objUIs.push_back(GetScene().Instantiate<Lobby>());
	data.objUIs.push_back(GetScene().Instantiate<LobbyTeamName>());
	data.objUIs.push_back(GetScene().Instantiate<LobbyPlayerNameOne>());
	data.objUIs.push_back(GetScene().Instantiate<LobbyPlayerNameTwo>());
	data.objUIs.push_back(GetScene().Instantiate<LobbyPlayerNameTree>());
}

/// @brief ゲームスタートページ
void TitlePageController::CreateGameStartPage()
{
	data.objUIs.push_back(GetScene().Instantiate<GameStart>());

	data.objUIs.push_back(GetScene().Instantiate<GameStartGuide>());
}
