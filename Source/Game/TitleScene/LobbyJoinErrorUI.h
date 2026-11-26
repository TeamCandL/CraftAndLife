#pragma once
#include <Game/Game.h>

struct LobbyJoinErrorUIData
{
	const Vec2 IMAGE_SIZE{ 100, 300 };
	Resource<Texture> texture{ "Image/Title/ErrorMessagePanel.png" };

	Component<UITransform> transform{ Vec2::Zero(), Vec2::Zero() };
	Component<UIImage> image{};
	Component<UIRaycastTarget> target{};
};

struct LobbyJoinErrorUI : ObjectBase<LobbyJoinErrorUIData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	/// @brief パネルが表示されているか取得する
	/// @return パネルが表示されている true / false
	bool IsShow();

	/// @brief パネルを表示する
	void Show();

	/// @brief パネルを非表示にする
	void Hide();
};