#include "pch\pch.h"
#include "MiniMap.h"
#include "MiniMapPoint.h"
#include "PlayerIcon2.h"
#include "PlayerIcon3.h"
#include "PlayerIcon4.h"

#include "../OtherPlayer.h"
#include "../OtherPlayerController.h"

#include "../EnemyController.h"
#include "../Enemy.h"
#include "../OtherEnemyController.h"
#include "../OtherEnemy.h"

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
	// 自分のプレイヤーの情報を取得
	Transform& playerTransform = Get<Transform>(objPlayer);

	Vec3 playerPosition = playerTransform.position;
	Vec3 angles         = playerTransform.rotation.ToEuler();

	float playerAngle = angles.y;

	// ----------------------------------------
	// 東西南北のマークを配置
	// ----------------------------------------
	for (float angle = 0; angle < DirectX::XM_2PI; angle += DirectX::XM_PIDIV2)
	{
		Get<MiniMapPoint>(points[static_cast<int>(angle / DirectX::XM_PIDIV2)]).SetPosition(angle - playerAngle, 150);
	}

	// ----------------------------------------
	// 仲間プレイヤーのアイコン更新
	// ----------------------------------------

	// 仲間プレイヤーを全員取得
	std::vector<GameObject> otherPlayers = Get<OtherPlayerController>(objOtherPlayerController).GetOtherPlayers();

	// 共通処理：仲間アイコン位置更新
	auto UpdateIcon = [&](GameObject otherPlayer, auto& icon)
	{
		if (otherPlayer.IsEmpty())
		{
			icon.SetPosition(0.0f, 10000.0f);
			return;
		}

		Vec3 otherPosition = Get<OtherPlayer>(otherPlayer).GetPosition();

		float deltaX = otherPosition.x - playerPosition.x;
		float deltaZ = otherPosition.z - playerPosition.z;

		float distance = std::sqrt(deltaX * deltaX + deltaZ * deltaZ);

		float iconAngle = std::atan2(-deltaZ, deltaX) - playerAngle;

		constexpr float viewDistance = 50.0f;
		constexpr float maxRadius    = 130.0f;
		constexpr float mapScale     = maxRadius / viewDistance;

		float radius = std::min(distance * mapScale, maxRadius);
		radius       = std::max(radius, 20.0f);

		icon.SetPosition(iconAngle, radius);
	};

	GameObject player1 = otherPlayers.size() > 0 ? otherPlayers[0] : GameObject{};

	GameObject player2 = otherPlayers.size() > 1 ? otherPlayers[1] : GameObject{};

	GameObject player3 = otherPlayers.size() > 2 ? otherPlayers[2] : GameObject{};

	UpdateIcon(player1, Get<PlayerIcon2>(objPlayerIcon2));
	UpdateIcon(player2, Get<PlayerIcon3>(objPlayerIcon3));
	UpdateIcon(player3, Get<PlayerIcon4>(objPlayerIcon4));

	// ----------------------------------------
	// 敵アイコンの共通更新処理
	// ----------------------------------------

	auto UpdateEnemyIcon = [&](size_t index, const Vec3& enemyPosition, bool isBoss, bool alive)
	{
		// アイコン配列を拡張
		if (enemyIcons.size() <= index)
		{
			enemyIcons.resize(index + 1);
			enemyIconIsBoss.resize(index + 1, false);
		}

		// 死亡した敵は非表示
		if (!alive)
		{
			if (!enemyIcons[index].IsEmpty())
			{
				Get<MiniMapPoint>(enemyIcons[index]).SetPosition(0.0f, 10000.0f);
			}

			return;
		}

		// ボスと通常敵でアイコンを作り直す
		if (!enemyIcons[index].IsEmpty() && enemyIconIsBoss[index] != isBoss)
		{
			GetScene().Destroy(enemyIcons[index]);

			enemyIcons[index] = GameObject{};
		}

		// アイコンが未生成なら生成
		if (enemyIcons[index].IsEmpty())
		{
			std::string imagePath = isBoss ? "Image/UI/BossIcon.png" : "Image/UI/EnemyIcon.png";

			// アイコンサイズ
			Vec2 iconSize = isBoss ? Vec2{ 55, 49 } : Vec2{ 16, 16 };

			enemyIcons[index] = GetScene().Instantiate<MiniMapPoint>(iconSize, imagePath, Vec2{ 1746, 174 });

			enemyIconIsBoss[index] = isBoss;
		}

		// プレイヤーからの距離と方向
		float deltaX = enemyPosition.x - playerPosition.x;
		float deltaZ = enemyPosition.z - playerPosition.z;

		float distance = std::sqrt(deltaX * deltaX + deltaZ * deltaZ);

		float iconAngle = std::atan2(-deltaZ, deltaX) - playerAngle;

		// ミニマップ表示範囲
		constexpr float viewDistance = 50.0f;
		constexpr float maxRadius    = 130.0f;
		constexpr float mapScale     = maxRadius / viewDistance;

		// 距離に応じてアイコン位置を計算
		float radius = std::min(distance * mapScale, maxRadius);

		radius = std::max(radius, 20.0f);

		// アイコン配置
		Get<MiniMapPoint>(enemyIcons[index]).SetPosition(iconAngle, radius);
	};

	// ----------------------------------------
	// ホスト側：EnemyControllerから敵を取得
	// ----------------------------------------

	if (!Data().objEnemyController.IsEmpty())
	{
		auto& enemyController = Get<EnemyController>(Data().objEnemyController);

		auto& allEnemies = enemyController.Data().allEnemys;

		for (size_t i = 0; i < allEnemies.size(); i++)
		{
			GameObject objEnemy = allEnemies[i];

			// 敵オブジェクトが存在しない
			if (objEnemy.IsEmpty())
			{
				UpdateEnemyIcon(i, Vec3::Zero(), false, false);

				continue;
			}

			Enemy& enemy = Get<Enemy>(objEnemy);

			// 敵情報を取得
			UpdateEnemyIcon(i, enemy.GetPosition(), enemy.Data().isBoss, enemy.Data().alive);
		}
	}

	// ----------------------------------------
	// ゲスト側：OtherEnemyControllerから敵を取得
	// ----------------------------------------

	else if (!Data().objOtherEnemyController.IsEmpty())
	{
		auto& otherEnemyController = Get<OtherEnemyController>(Data().objOtherEnemyController);

		auto& otherEnemies = otherEnemyController.Data().idToObjOtherEnemy;

		size_t index = 0;

		for (auto& [enemyId, objOtherEnemy] : otherEnemies)
		{
			// 敵オブジェクトが存在しない
			if (!objOtherEnemy)
			{
				UpdateEnemyIcon(index, Vec3::Zero(), false, false);

				index++;

				continue;
			}

			OtherEnemy& enemy = Get<OtherEnemy>(objOtherEnemy);

			// ボス判定
			bool isBoss = otherEnemyController.IsBossEnemy(enemy.Data().enemyType);

			// ゲスト側の敵位置
			Transform& enemyTransform = Get<Transform>(objOtherEnemy);
			Vec3 enemyPosition        = enemyTransform.position;

			// アイコン更新
			UpdateEnemyIcon(index, enemyPosition, isBoss, !enemy.Data().isDead);

			index++;
		}
	}
}

// ----------------------------------------
// EnemyController受け取り
// ----------------------------------------
void MiniMap::SetEnemyController(
    GameObject obj)
{
	Data().objEnemyController = obj;
}

// ----------------------------------------
// OtherEnemyController受け取り
// ----------------------------------------
void MiniMap::SetOtherEnemyController(
    GameObject obj)
{
	Data().objOtherEnemyController = obj;
}