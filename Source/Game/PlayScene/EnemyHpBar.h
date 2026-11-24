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
		if (maxHp > 0)
		{
			hpRatio = static_cast<float>(hp) / static_cast<float>(maxHp);
		}
	}

	// --- 背景用（後ろ・レイヤー9999） ---
	Resource<Texture> bgTexture{ "Image/UI/self_info.png" };
	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 240, 65 }, UILayerOrder{ 1 }.WithUser(9999) };
	Component<UIImage> backgroundImage{ bgTexture };

	/*Resource<Texture> bgTexture{ "Image/UI/Mygauge.png" };
	Component<UITransform> backgroundTransform{ Vec2::Zero(), Vec2{ 240, 65 }, UILayerOrder{}.WithUser(9999) };
	Component<UIImage> backgroundImage{ bgTexture };*/

	// --- 減るゲージ（バー）用（手前・レイヤー10000） ---
	Resource<Texture> barTexture{ "Image/UI/BossGauge.png" };
	Component<UITransform> barTransform{ Vec2::Zero(), Vec2{ 200, 16 }, UILayerOrder{ 2 }.WithUser(10000) };
	Component<UIImage> barImage{ barTexture };

	/*Resource<Texture> barTexture{ "Image/UI/self_info.png" };
	Component<UITransform> barTransform{ Vec2::Zero(), Vec2{ 200, 16 }, UILayerOrder{}.WithUser(10000) };
	Component<UIImage> barImage{ barTexture };*/
	GameObject ownerEnemy;
	int32_t hp{};
	int32_t maxHp{ 100 };
	float hpRatio{ 1.0f };     // HPの割合（0.0f〜1.0f）
	float maxWidth{ 200.0f };  // ゲージの最大幅

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