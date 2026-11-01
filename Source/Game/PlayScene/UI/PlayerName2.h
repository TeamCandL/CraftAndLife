
#pragma once
#include <Game/Game.h>

struct PlayerName2Data
{
	// 表示するプレイヤーの名前
	std::string playerName{};

	// 名前を表示する対象のプレイヤー
	GameObject targetPlayer{};

	// 背景画像
	Resource<Texture> texture{ "Image/UI/PlayerName_background.png" };

	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 207, 60 }, UILayerOrder{}.WithUser(9999) };

	Component<UIImage> image{ texture };

	// 名前のフォント
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(36)
		                     .WithFontAlignment(FontAlignment_Center) };

	// 名前のテキスト
	Component<UITransform> textTransform{ Vec2::Zero(), Vec2{ 192, 27 }, UILayerOrder{}.WithUser(10000) };

	Component<UIText> text{ font, Color::WHITE };

	// コンストラクタ
	PlayerName2Data(
	    const std::string& _name,
	    GameObject _targetPlayer) :
	    playerName{ _name },
	    targetPlayer{ _targetPlayer }
	{
	}
};

struct PlayerName2 : ObjectBase<PlayerName2Data>
{
	using ObjectBase<PlayerName2Data>::ObjectBase;

	void Start() override;
	void Update() override;
};
