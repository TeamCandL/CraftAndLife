#include "pch\pch.h"
#include "MiniMap.h"
#include "MiniMapPoint.h"
#include "PlayerIcon2.h"
#include "../OtherPlayer.h"
#include "../OtherPlayerController.h"

void MiniMap::Start()
{
	Vec2 centerPosition{ 1746, 174 };
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_east.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_south.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_west.png", centerPosition));
	points.push_back(
	    GetScene().Instantiate<MiniMapPoint>(Vec2{ 38, 38 }, "Image/UI/mini_map_north.png", centerPosition));

	for (float angle = 0; angle < DirectX::XM_2PI; angle += DirectX::XM_PIDIV2)
	{
		Get<MiniMapPoint>(points[static_cast<int>(angle / DirectX::XM_PIDIV2)]).SetPosition(angle, 150);
	}

	objPlayerIcon2 = GetScene().Instantiate<PlayerIcon2>();
}

void MiniMap::Update()
{
	// =====================================
	// 自分のプレイヤーの情報を取得
	// =====================================

	Transform& playerTransform = Get<Transform>(objPlayer);

	Vec3 playerPosition = playerTransform.position;

	Vec3 angles       = playerTransform.rotation.ToEuler();
	float playerAngle = angles.y;

	// =====================================
	// 東西南北のマークを配置
	// =====================================

	for (float angle = 0; angle < DirectX::XM_2PI; angle += DirectX::XM_PIDIV2)
	{
		Get<MiniMapPoint>(points[static_cast<int>(angle / DirectX::XM_PIDIV2)]).SetPosition(angle - playerAngle, 150);
	}

	// =====================================
	// 仲間プレイヤーを取得
	// =====================================

	GameObject otherPlayer = Get<OtherPlayerController>(objOtherPlayerController).GetFirstOtherPlayer();

	// 仲間が存在しない場合
	if (otherPlayer.IsEmpty())
	{
		if (!objPlayerIcon2.IsEmpty())
		{
			Get<PlayerIcon2>(objPlayerIcon2).SetPosition(0.0f, 10000.0f);
		}

		return;
	}

	// =====================================
	// 仲間プレイヤーの座標を取得
	// =====================================

	Vec3 otherPosition = Get<OtherPlayer>(otherPlayer).GetPosition();

	// =====================================
	// 自分と仲間の相対座標を計算
	// =====================================

	float deltaX = otherPosition.x - playerPosition.x;
	float deltaZ = otherPosition.z - playerPosition.z;

	// 仲間までの距離
	float distance = std::sqrt(deltaX * deltaX + deltaZ * deltaZ);

	// =====================================
	// 自分の回転を考慮した角度を計算
	// =====================================

	float iconAngle = std::atan2(-deltaZ, deltaX) - playerAngle;

	// =====================================
	// ミニマップ上の距離に変換
	// =====================================

	constexpr float mapScale = 0.1f;

	float radius = std::min(distance * mapScale, 130.0f);

	// =====================================
	// 仲間アイコンを生成
	// =====================================

	if (objPlayerIcon2.IsEmpty())
	{
		objPlayerIcon2 = GetScene().Instantiate<PlayerIcon2>();
	}

	// =====================================
	// 仲間アイコンの位置を更新
	// =====================================

	Get<PlayerIcon2>(objPlayerIcon2).SetPosition(iconAngle, radius);
}
