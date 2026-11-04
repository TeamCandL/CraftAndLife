#pragma once
#include <Game/Game.h>

struct BossGaugeRData
{
	BossGaugeRData(
	    GameObject objBossInfo_)
	{
		objBossInfo = objBossInfo_;
	}

	Resource<Texture> texture{ "Image/UI/BossGaugeR.png" };

	Component<UITransform> transform{ Vec2{ 672, 50 }, Vec2{ 547, 38 } };
	Component<UIImage> image{ texture };

	GameObject objBossInfo{};  ///< ボスのHP数値
};

struct BossGaugeR : ObjectBase<BossGaugeRData>
{
	using ObjectBase<BossGaugeRData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
