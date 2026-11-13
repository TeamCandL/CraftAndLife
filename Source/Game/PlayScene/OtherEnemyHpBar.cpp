#include "pch\pch.h"
#include "OtherEnemyHpBar.h"
#include "OtherEnemy.h"
#include "../Manager/SessionManager/SessionStateReader.h"

void OtherEnemyHpBar::Start() {}

void OtherEnemyHpBar::Update()
{
	// 敵から最新のHPと座標を同期
	if (!ownerEnemy.IsEmpty())
	{
		OtherEnemy& enemy                          = Get<OtherEnemy>(ownerEnemy);
		SessionManager& sessionManager             = Get<SessionManager>();
		const InSessionStatus::EnemyStatus& status = sessionManager.RefEnemyStatus(enemy.Data().enemyId);
		SessionStateReader reader(sessionManager);

		StateTarget target = StateTarget::Enemy(enemy.Data().enemyId);
		int32_t _hp        = 0;
		reader.TryGetState(target, "hp", _hp);

		SetTargetPosition(status.position);
		SetHp(_hp);

		// if (maxHp > 0)
		//{
		//	hpRatio = static_cast<float>(hp) / static_cast<float>(maxHp);
		//	hpRatio = std::clamp(hpRatio, 0.0f, 1.0f);
		// }
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

	// 6. 背景の位置とサイズ設定（240×65）
	float bgWidth  = 240.0f;
	float bgHeight = 65.0f;
	float bgX      = screenPos.x - (bgWidth * 0.5f);
	float bgY      = screenPos.y - (bgHeight * 0.5f);

	backgroundTransform.position = Vec2{ bgX, bgY };
	backgroundTransform.scale    = Vec2{ bgWidth, bgHeight };

	// 7. バーの位置とスケール設定（背景の穴の位置に合わせて微調整）
	float barWidth  = 200.0f;
	float barHeight = 16.0f;
	float barX      = screenPos.x - barWidth * 0.5f;
	float barY      = screenPos.y - barHeight * 0.5f;

	barTransform.position = Vec2{ barX, barY };
	barTransform.scale    = Vec2{ barWidth * hpRatio, barHeight };  // HP割合に応じて横幅が縮む

	/*ImGui::Begin("BarUIPos");
	ImGui::DragFloat2("barTransform : ", barTransform.position.v);
	ImGui::DragFloat2("bgTransform : ", backgroundTransform.position.v);
	ImGui::End();*/

	if (hp <= 0)
	{
		DestroyMe();
	}
}

void OtherEnemyHpBar::SetTargetPosition(
    const Vec3& _position)
{
	m_worldPosition = _position;
}

void OtherEnemyHpBar::SetHp(
    const int32_t _hp)
{
	hp = _hp;
}
