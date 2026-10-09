#pragma once
#include <Game/Game.h>

/// @brief 合言葉入力パネルのデータ
struct WordInputPanelData
{
	const Vec2 IMAGE_SIZE{ 1920, 1080 };  ///< 画像サイズ(ぴくせる)
	
	Resource<Texture> texture{ "Image/Title/WordInputPanel.png" };  ///< 画像

	Component<UITransform> transform{ Vec2::Zero(), Vec2::Zero() };  ///< UI座標系
	Component<UIImage> image{};                                            ///< UI画像
	Component<UIRaycastTarget> target{};                                   ///< UI当たり判定
};

/// @brief 合言葉入力パネル
struct WordInputPanel : ObjectBase<WordInputPanelData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief パネルが表示されているか取得する
	/// @return パネルが表示されている true / false
	bool IsShown();

	/// @brief パネルを表示する
	void Show();

	/// @brief パネルを非表示にする
	void Hide();
};
