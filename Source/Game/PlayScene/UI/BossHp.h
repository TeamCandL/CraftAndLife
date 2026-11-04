#pragma once
#include <Game/Game.h>

struct BossHpData
{
	BossHpData(
	    GameObject objBossInfo_)
	{
		objBossInfo = objBossInfo_;
	}

	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIText> text{ font, Color::WHITE };

	GameObject objBossInfo{};  ///< ボスの情報
};

struct BossHp : ObjectBase<BossHpData>
{
	using ObjectBase<BossHpData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
