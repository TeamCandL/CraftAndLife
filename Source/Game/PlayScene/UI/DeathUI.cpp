#include "pch\pch.h"
#include "DeathUI.h"

#include "../OtherPlayer.h"
#include "../Player.h"
#include "../Player/HelthState.h"

#include <Base/Manager/ScreenSize.h>
#include <Game/System/CameraSystem.h>

void DeathUI::Start() {}

void DeathUI::Update()
{
	// 対象プレイヤーが存在するか確認
	if (targetPlayer.IsEmpty())
	{
		transform.scale = Vec2::Zero();
		return;
	}

	Vec3 playerPosition{};

	// =====================================
	// 自分のプレイヤーの場合
	// =====================================
	if (isLocalPlayer)
	{
		Player& player = Get<Player>(targetPlayer);

		// 生存状態なら非表示
		if (player.Data().helthState != HelthState_Dead)
		{
			transform.scale = Vec2::Zero();
			return;
		}

		playerPosition = player.GetPosition();
	}
	// =====================================
	// 他プレイヤーの場合
	// =====================================
	else
	{
		OtherPlayer& player = Get<OtherPlayer>(targetPlayer);

		// 生存状態なら非表示
		if (!player.IsDead())
		{
			transform.scale = Vec2::Zero();
			return;
		}

		playerPosition = player.GetPosition();
	}

	// =====================================
	// 死亡中なら表示
	// =====================================

	transform.scale = Vec2{ 70.0f, 83.0f };

	// PlayerNameと同じ頭の高さ
	playerPosition.y += 1.7f;

	// =====================================
	// カメラ情報を取得
	// =====================================

	const CameraSystemData& camera = Get<CameraSystem>().Data();

	Vec2 screenSize = Get<ScreenSize>();

	// =====================================
	// 3D座標を2D画面座標に変換
	// =====================================

	using namespace DirectX;

	XMVECTOR worldPosition = XMVectorSet(playerPosition.x, playerPosition.y, playerPosition.z, 1.0f);

	XMVECTOR projectedPosition = XMVector3Project(
	    worldPosition,
	    0.0f,
	    0.0f,
	    screenSize.x,
	    screenSize.y,
	    0.0f,
	    1.0f,
	    camera.projectionMatrix,
	    camera.viewMatrix,
	    XMMatrixIdentity());

	XMFLOAT3 screenPosition{};

	XMStoreFloat3(&screenPosition, projectedPosition);

	// カメラの範囲外なら非表示
	if (screenPosition.z < 0.0f || screenPosition.z > 1.0f)
	{
		transform.scale = Vec2::Zero();
		return;
	}

	// =====================================
	// DeathUIの画面上の位置を設定
	// =====================================

	transform.position = Vec2{ screenPosition.x - 35.0f, screenPosition.y - 130.0f };
}