#pragma once
#include <Game/Game.h>
#include <chrono>

struct HarvestableGaugeData
{
	// ゲージを表示する対象の採取オブジェクト
	GameObject targetHarvestable{};

	// 背景画像
	Resource<Texture> backgroundTexture{ "Image/UI/HarvestableGauge_Background.png" };

	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 160, 9 }, UILayerOrder{}.WithUser(9999) };

	Component<UIImage> backgroundImage{ backgroundTexture };

	// 耐久値を表示するゲージ画像
	Resource<Texture> fillTexture{ "Image/UI/HarvestableGauge_Fill.png" };

	Component<UITransform> fillTransform{ Vec2::Zero(), Vec2{ 160, 9 }, UILayerOrder{}.WithUser(10000) };

	Component<UIImage> fillImage{ fillTexture };

	// コンストラクタ
	HarvestableGaugeData(
	    GameObject _targetHarvestable) :
	    targetHarvestable{ _targetHarvestable }
	{
	}

	// 最後に攻撃された時刻
	std::chrono::steady_clock::time_point lastHitTime{ std::chrono::steady_clock::now() };
};

struct HarvestableGauge : ObjectBase<HarvestableGaugeData>
{
	using ObjectBase<HarvestableGaugeData>::ObjectBase;

	void Start() override;
	void Update() override;

	// ゲージの表示対象を変更
	void SetTarget(GameObject _targetHarvestable);
};
