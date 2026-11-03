#pragma once
#include <Game/Game.h>

struct BossHpData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(32)
		                     .WithFontAlignment(FontAlignment_Center) };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIText> text{ font, Color::WHITE };

	GameObject objBoss{};        ///< ボスのオブジェクト
	int32_t maxHP{};             ///< ボスの最大HP
	int32_t hp{};                ///< ボスの現在HP
	float rateOfHp{};            ///< ボスのHPの割合
	bool isBossActive{ false };  ///< ボスがアクティブかどうか
};

struct BossHp : ObjectBase<BossHpData>
{
	using ObjectBase<BossHpData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	void SetBoss(
	    GameObject objBoss_)
	{
		isBossActive = true;
		objBoss      = objBoss_;
	}

	float GetRateOfHp() const
	{
		return rateOfHp;
	}
};
