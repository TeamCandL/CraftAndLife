#pragma once
#include <Game/Game.h>
#include <DirectXMath.h>

struct EnemyHpBarData
{
	EnemyHpBarData() = default;
	EnemyHpBarData(
	    const Vec3& initialPos)
	{
	}
	EnemyHpBarData(
	    GameObject _enemy,
	    const Vec3& _initialPos,
	    int32_t _hp,
	    int32_t _maxHp) :
	    ownerEnemy(_enemy),
	    hp(_hp),
	    maxHp(_maxHp),
	    m_worldPosition(_initialPos)
	{
	}

	// --- 背景用 ---
	Resource<Texture> bgTexture{ "Image/UI/self_info.png" };
	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 240, 65 }, UILayerOrder{}.WithUser(9999) };
	Component<UIImage> backgroundImage{ bgTexture };

	// --- 減るゲージ（バー）用 ---
	// ※バー用の画像パスは、ご自身のプロジェクトのゲージ画像に合わせて変更してください
	Resource<Texture> barTexture{ "Image/UI/Mygauge.png" };
	Component<UITransform> barTransform{ Vec2{ 0, 0 }, Vec2{ 100, 16 }, UILayerOrder{}.WithUser(10000) };
	Component<UIImage> barImage{ barTexture };

	GameObject ownerEnemy;
	int32_t hp{};
	int32_t maxHp{};
	float hpRatio{ 1.0f };  // HPの割合（0.0f〜1.0f）

	Vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
	Vec3 m_offset{ 0.0f, 2.2f, 0.0f };  // 頭上の高さ
};

struct EnemyHpBar : ObjectBase<EnemyHpBarData>
{
	using ObjectBase<EnemyHpBarData>::ObjectBase;

	void Start() override;
	void Update() override;

	void SetTargetPosition(const Vec3& _position);
	void SetHp(const int32_t _hp);
};