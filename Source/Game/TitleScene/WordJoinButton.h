#pragma once
#include <Game/Game.h>
#include <ImGui/imgui_stdlib.h>

/// @brief 合言葉で参加するボタンのデータ
struct WordJoinButtonData
{
	WordJoinButtonData(
	    const GameObject _objWordInputPanel) :
	    objWordInputPanel{ _objWordInputPanel }
	{
	}

	GameObject objWordInputPanel{};  ///< 合言葉入力パネルオブジェクト

	GameObject objButton{};                                              ///< ボタンオブジェクト
	std::string_view imageFileName{ "Image/Title/WordJoinButton.png" };  ///< ボタン画像のファイルパス
	Vec2 begin{ 720, 840 };                                              ///< ボタンの開始位置ピクセル
	Vec2 scale{ 482, 142 };                                              ///< ボタンのサイズピクセル

	bool isInputtingWord{ false };
	std::string inputWord{};
	std::string inputPlayerName{};

	Resource<Audio> pushSound{ "Sound/stopbutton.mp3" };
};

/// @brief 合言葉で参加するボタン
struct WordJoinButton : ObjectBase<WordJoinButtonData>
{
	using ObjectBase<WordJoinButtonData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
