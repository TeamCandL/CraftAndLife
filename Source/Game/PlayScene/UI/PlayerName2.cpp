
#include "pch\pch.h"
#include "PlayerName2.h"

#include "../OtherPlayer.h"
#include <Base/Manager/ScreenSize.h>

void PlayerName2::Start()
{
	// 表示する名前を設定
	text.content = playerName;
}

void PlayerName2::Update()
{
	// 対象プレイヤーが存在しない場合
	if (targetPlayer.IsEmpty())
	{
		return;
	}

	// 他プレイヤーのワールド座標を取得
	Vec3 playerPosition = Get<OtherPlayer>(targetPlayer).GetPosition();

	// 頭上に表示するための高さ
	Vec3 worldPosition{ playerPosition.x, playerPosition.y + 250.0f, playerPosition.z };

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

	// カメラの後ろにいる場合は表示しない
	if (screenPos.z < 0.0f || screenPos.z > 1.0f)
	{
		backgroundTransform.scale = Vec2::Zero();
		textTransform.scale       = Vec2::Zero();
		return;
	}

	// UIサイズを元に戻す
	backgroundTransform.scale = Vec2{ 207, 60 };
	textTransform.scale       = Vec2{ 192, 27 };

	// 背景画像の位置
	backgroundTransform.position = Vec2{ screenPos.x - 103.5f, screenPos.y - 60.0f };

	// テキストの位置
	textTransform.position = Vec2{ screenPos.x - 96.0f, screenPos.y - 43.5f };
}
