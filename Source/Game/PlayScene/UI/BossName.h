#pragma once
#include <Game/Game.h>

struct BossNameData
{
	BossNameData(
	    GameObject objBossInfo_)
	{
		objBossInfo = objBossInfo_;
	}
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 657, 0 }, Vec2{ 577, 50 } };
	Component<UIText> text{ font, Color::WHITE };

	GameObject objBossInfo{};  ///< ボスのHP数値
};

struct BossName : ObjectBase<BossNameData>
{
	using ObjectBase<BossNameData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
