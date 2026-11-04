
#pragma once

#include <Game/Game.h>
#include <vector>

/// @brief ページの種類
enum class PageType
{
	Title,       // タイトル
	TeamSelect,  // チーム選択
	PlayerName,  // プレイヤー名入力
	Lobby,       // ロビー
	GameStart    // ゲームスタート
};

/// @brief ページ管理データ
struct TitlePageControllerData
{
	// 現在表示しているUI要素
	std::vector<GameObject> objUIs{};
};

/// @brief タイトル画面のページ管理
struct TitlePageController : ObjectBase<TitlePageControllerData>
{
	void Start() override
	{
		// 初期ページはタイトルページ
		ChangePage(PageType::Title);
	}
	/// @brief ページ変更
	/// @param page 表示したいページ
	void ChangePage(PageType page);

	TitlePageControllerData data{};

	/// @brief 現在のページのUIを削除
	void ClearPage();

	/// @brief 各ページのUI生成
	void CreateTitlePage();
	void CreateTeamSelectPage();
	void CreatePlayerNamePage();
	void CreateLobbyPage();
	void CreateGameStartPage();
};
