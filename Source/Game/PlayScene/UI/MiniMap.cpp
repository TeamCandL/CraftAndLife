#include "pch\pch.h"
#include "MiniMap.h"
#include "MiniMapPoint.h"
#include "PlayerIcon2.h"
#include "PlayerIcon3.h"
#include "PlayerIcon4.h"
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

	// 仲間プレイヤー用アイコンを3つ生成
	objPlayerIcon2 = GetScene().Instantiate<PlayerIcon2>();
	objPlayerIcon3 = GetScene().Instantiate<PlayerIcon3>();
	objPlayerIcon4 = GetScene().Instantiate<PlayerIcon4>();
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
	// 仲間プレイヤーを全員取得
	// =====================================

	std::vector<GameObject> otherPlayers = Get<OtherPlayerController>(objOtherPlayerController).GetOtherPlayers();

	// =====================================
	// アイコンの位置を更新する共通処理
	// =====================================

	auto UpdateIcon = [&](GameObject otherPlayer, auto& icon)
	{
		// 仲間が存在しない場合は画面外に移動
		if (otherPlayer.IsEmpty())
		{
			icon.SetPosition(0.0f, 10000.0f);
			return;
		}

		// 仲間の座標を取得
		Vec3 otherPosition = Get<OtherPlayer>(otherPlayer).GetPosition();

		// 自分と仲間の相対座標
		float deltaX = otherPosition.x - playerPosition.x;
		float deltaZ = otherPosition.z - playerPosition.z;

		// 仲間までの距離
		float distance = std::sqrt(deltaX * deltaX + deltaZ * deltaZ);

		// 自分の回転を考慮した角度
		float iconAngle = std::atan2(-deltaZ, deltaX) - playerAngle;

		// ミニマップ上の距離に変換
		constexpr float viewDistance = 50.0f;
		constexpr float maxRadius    = 130.0f;

		constexpr float mapScale = maxRadius / viewDistance;

		float radius = std::min(distance * mapScale, maxRadius);

		// 自分の矢印と重ならないようにする
		radius = std::max(radius, 20.0f);

		// アイコンの位置を更新
		icon.SetPosition(iconAngle, radius);
	};

	// =====================================
	// 1人目・2人目・3人目のアイコンを更新
	// =====================================

	GameObject player1 = otherPlayers.size() > 0 ? otherPlayers[0] : GameObject{};

	GameObject player2 = otherPlayers.size() > 1 ? otherPlayers[1] : GameObject{};

	GameObject player3 = otherPlayers.size() > 2 ? otherPlayers[2] : GameObject{};

	UpdateIcon(player1, Get<PlayerIcon2>(objPlayerIcon2));
	UpdateIcon(player2, Get<PlayerIcon3>(objPlayerIcon3));
	UpdateIcon(player3, Get<PlayerIcon4>(objPlayerIcon4));
}