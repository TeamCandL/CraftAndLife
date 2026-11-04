#pragma once
#include <Game/Game.h>

struct BossGaugeData
{
	BossGaugeData(
	    GameObject _objBossHp) :
	    objBossHp{ _objBossHp }
	{
	}

	Resource<Texture> texture{ "Image/UI/BossGauge.png" };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIImage> image{ texture };

	float rateOfHp{};        ///< ボスのHPの割合
	GameObject objBossHp{};  ///< ボスのHP数値
};

struct BossGauge : ObjectBase<BossGaugeData>
{
	using ObjectBase<BossGaugeData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
