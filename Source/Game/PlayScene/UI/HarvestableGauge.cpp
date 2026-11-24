#include "pch\pch.h"
#include "HarvestableGauge.h"

#include "../Harvestable.h"
#include <Base/Manager/ScreenSize.h>

void HarvestableGauge::Start()
{
	// 初期状態ではゲージを非表示にする
	backgroundTransform.scale = Vec2::Zero();
	fillTransform.scale       = Vec2::Zero();
}

void HarvestableGauge::Update()
{
	// 対象の採取オブジェクトが存在しない場合
	if (targetHarvestable.IsEmpty())
	{
		backgroundTransform.scale = Vec2::Zero();
		fillTransform.scale       = Vec2::Zero();
		return;
	}

	// 最後の攻撃から2秒経過したら非表示
	float elapsedTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - lastHitTime).count();

	if (elapsedTime >= 2.0f)
	{
		backgroundTransform.scale = Vec2::Zero();
		fillTransform.scale       = Vec2::Zero();
		return;
	}

	// 採取オブジェクトの情報を取得
	Harvestable& harvestable = Get<Harvestable>(targetHarvestable);

	// すでに採取済みなら非表示
	if (harvestable.IsHarvested())
	{
		backgroundTransform.scale = Vec2::Zero();
		fillTransform.scale       = Vec2::Zero();
		return;
	}

	// 採取オブジェクトのワールド座標を取得
	Vec3 objectPosition = harvestable.GetPosition();

	// オブジェクトの上に表示するための高さ
	Vec3 worldPosition{ objectPosition.x, objectPosition.y + 2.0f, objectPosition.z };

	// カメラ情報を取得
	const CameraSystemData& camera = Get<CameraSystem>().Data();

	// スクリーンサイズを取得
	Vec2 screenSize = Get<ScreenSize>();

	// ワールド座標
	DirectX::XMFLOAT3 worldPos{ worldPosition.x, worldPosition.y, worldPosition.z };

	// 変換後のスクリーン座標
	DirectX::XMFLOAT3 screenPos{};

	// 3D座標を2D座標に変換
	DirectX::XMVECTOR projectedPosition = DirectX::XMVector3Project(
	    DirectX::XMLoadFloat3(&worldPos),
	    0.0f,
	    0.0f,
	    screenSize.x,
	    screenSize.y,
	    0.0f,
	    1.0f,
	    camera.projectionMatrix,
	    camera.viewMatrix,
	    DirectX::XMMatrixIdentity());

	DirectX::XMStoreFloat3(&screenPos, projectedPosition);

	// カメラの後ろにある場合は非表示
	if (screenPos.z < 0.0f || screenPos.z > 1.0f)
	{
		backgroundTransform.scale = Vec2::Zero();
		fillTransform.scale       = Vec2::Zero();
		return;
	}

	// 耐久値の割合を計算
	float durabilityRate = 0.0f;

	if (harvestable.GetMaxDurability() > 0)
	{
		durabilityRate =
		    static_cast<float>(harvestable.GetDurability()) / static_cast<float>(harvestable.GetMaxDurability());
	}

	// 0.0～1.0の範囲に制限
	durabilityRate = std::clamp(durabilityRate, 0.0f, 1.0f);

	// ゲージの最大サイズ
	const float maxWidth = 160.0f;
	const float height   = 9.0f;

	// 背景の位置とサイズ
	backgroundTransform.position = Vec2{ screenPos.x - maxWidth / 2.0f, screenPos.y };

	backgroundTransform.scale = Vec2{ maxWidth, height };

	// 緑色のゲージの位置とサイズ
	fillTransform.position = backgroundTransform.position;

	fillTransform.scale = Vec2{ maxWidth * durabilityRate, height };
}

void HarvestableGauge::SetTarget(
    GameObject _targetHarvestable)
{
	targetHarvestable = _targetHarvestable;

	// 攻撃された時刻を更新
	lastHitTime = std::chrono::steady_clock::now();
}
