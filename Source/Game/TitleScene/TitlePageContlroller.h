
#pragma once

#include <Game/Game.h>

struct TitlePageController
{

	enum class Page
	{
		Title,       // タイトル
		TeamSelect,  // チーム選択
		PlayerName,  // プレイヤー名入力
		Lobby,       // ロビー
		GameStart    // ゲームスタート
	};
	// ページ変更
	void ChangePage(Page page);

	Page currentPage = Page::Title;

	// 現在のページを削除
	void ClearPage();

	// 各ページの生成
	void CreateTitlePage();
	void CreateTeamSelectPage();
	void CreatePlayerNamePage();
	void CreateLobbyPage();
	void CreateGameStartPage();
};

struct TitlePageControllerComponent : ComponentBase<TitlePageController>
{
};
