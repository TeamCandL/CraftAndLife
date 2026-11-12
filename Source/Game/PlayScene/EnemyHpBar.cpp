#include "pch\pch.h"
#include "EnemyHpBar.h"
#include "Enemy.h"
#include <Game/System/CameraSystem.h>
#include <Base/Manager/ScreenSize.h>
#include "../Manager/SessionManager.h"

void EnemyHpBar::Start()
{
	// 初期化処理（必要であれば）
}

void EnemyHpBar::Update()
{
	Enemy& enemy = Get<Enemy>(ownerEnemy);
	SetHp(enemy.Data().hp);
	SetTargetPosition(enemy.GetPosition());

	int _active{ 255 };
	int HP{ hp };
	ImGui::Begin("EnemyHpBar");
	ImGui::DragInt("active = 255 : ", &_active);
	ImGui::DragFloat3("position:", m_worldPosition.v);
	ImGui::DragInt("hp : ", &HP);
	ImGui::End();

	SessionManager& sessionManager = Get<SessionManager>();
	if (sessionManager.RefStatus().imLobbyHost)
	{
		Enemy& enemy = Get<Enemy>(ownerEnemy);
		hp           = enemy.Data().hp;
	}

	// 1. 敵のワールド座標に頭上オフセットを加算
	Vec3 worldPosition = m_worldPosition + m_offset;

	// 2. カメラ情報と画面サイズの取得
	const CameraSystemData& camera = Get<CameraSystem>().Data();
	Vec2 screenSize                = Get<ScreenSize>();

	// 3. 3D座標をDirectX::XMFLOAT3に変換
	DirectX::XMFLOAT3 worldPos{ worldPosition.x, worldPosition.y, worldPosition.z };
	DirectX::XMFLOAT3 screenPos{};

	// 4. XMVector3Projectで2D画面座標に変換
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

	// 5. カメラの範囲外（背後など）にいる場合は非表示にする
	if (screenPos.z < 0.0f || screenPos.z > 1.0f)
	{
		backgroundTransform.scale = Vec2::Zero();
		barTransform.scale        = Vec2::Zero();
		return;
	}

	// 6. 画面上の中心位置を合わせるオフセット調整（例：幅100なので半分の50を引く）
	float offsetX = 50.0f;
	float offsetY = 8.0f;

	backgroundTransform.position = Vec2{ screenPos.x - offsetX, screenPos.y - offsetY };
	barTransform.position        = Vec2{ screenPos.x - offsetX, screenPos.y - offsetY };

	// 7. サイズと、HP割合に応じたバーの横幅変更
	backgroundTransform.scale = Vec2{ 100.0f, 16.0f };
	barTransform.scale        = Vec2{ 100.0f * hpRatio, 16.0f };
}

void EnemyHpBar::SetTargetPosition(
    const Vec3& position)
{
	m_worldPosition = position;
}

void EnemyHpBar::SetHp(
    const int32_t _hp)
{
	hp      = _hp;
	hpRatio = hp / maxHp;
}
