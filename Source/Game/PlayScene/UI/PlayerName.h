#pragma once
#include <Game/Game.h>

struct PlayerNameData
{
	// 表示するプレイヤーの名前
	std::string playerName{};

	// 名前を表示する対象のプレイヤー
	GameObject targetPlayer{};

	// 背景画像
	Resource<Texture> texture{ "Image/UI/PlayerName_background.png" };

	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 240, 65 }, UILayerOrder{}.WithUser(9999) };

	Component<UIImage> image{ texture };

	// 名前のフォント
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(24)
		                     .WithFontAlignment(FontAlignment_Center) };

	// 名前のテキスト
	Component<UITransform> textTransform{ Vec2::Zero(), Vec2{ 220, 40 }, UILayerOrder{}.WithUser(10000) };

	Component<UIText> text{ font, Color::WHITE };

	// コンストラクタ
	PlayerNameData(
	    const std::string& _name,
	    GameObject _targetPlayer) :
	    playerName{ _name },
	    targetPlayer{ _targetPlayer }
	{
	}
};

struct PlayerName : ObjectBase<PlayerNameData>
{
	using ObjectBase<PlayerNameData>::ObjectBase;

	void Start() override;
	void Update() override;
};
