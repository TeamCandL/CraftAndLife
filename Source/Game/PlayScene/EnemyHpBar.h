#pragma once
#include <Game/Game.h>
#include <DirectXMath.h>
#include <Game/Component/UI/UIWorldBinder.h>

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

	const int32_t BAR_WIDTH  = 200;  // ゲージの最大幅
	const int32_t BAR_HEIGHT = 30;   // ゲージの高さ
	// --- 減るゲージ（バー）用（手前・レイヤー10000） ---
	Resource<Texture> barTexture{ "Image/UI/EnemyHpBar.png" };
	Component<UITransform> barTransform{ Vec2{ -BAR_WIDTH / 2, -BAR_HEIGHT / 2 },
		                                 Vec2{ BAR_WIDTH, BAR_HEIGHT },
		                                 UILayerOrder{ 1 }.WithUser(1) };
	Component<UIImage> barImage{ barTexture };
	Component<Transform> transform{};
	Component<UIWorldBinder> uiWorldBinder{ barTransform, transform, 0.0f };

	GameObject ownerEnemy;
	GameObject objBackGround;
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
	void OnDestroy() override;

	void SetTargetPosition(const Vec3& _position);
	void SetHp(const int32_t _hp);
};