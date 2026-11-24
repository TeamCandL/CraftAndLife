#pragma once
#include <Game/Game.h>
#include <DirectXMath.h>
#include <Game/Component/UI/UIWorldBinder.h>

struct OtherEnemyHpBarBgData
{
	OtherEnemyHpBarBgData() = default;
	OtherEnemyHpBarBgData(
	    const Vec3& initialPos)
	{
	}
	OtherEnemyHpBarBgData(
	    GameObject _hpBar,
	    const Vec3& _initialPos) :
	    ownerOtherEnemyHp(_hpBar),
	    m_worldPosition(_initialPos)
	{
	}

	const int32_t BG_WIDTH{ 210 };  // 背景の最大幅
	const int32_t BG_HEIGHT{ 44 };  // 背景の高さ
	// --- 背景用（後ろ・レイヤー9999） ---
	Resource<Texture> bgTexture{ "Image/UI/EnemyHpBack.png" };
	Component<UITransform> backgroundTransform{ Vec2{ -BG_WIDTH / 2, -BG_HEIGHT / 2 },
		                                        Vec2{ BG_WIDTH, BG_HEIGHT },
		                                        UILayerOrder{ 1 }.WithUser(9999) };
	Component<UIImage> backgroundImage{ bgTexture };
	Component<Transform> transform{};
	Component<UIWorldBinder> uiWorldBinder{ backgroundTransform, transform, 10.0f };

	GameObject ownerOtherEnemyHp;
	float maxWidth{ 200.0f };  // ゲージの最大幅

	Vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
	Vec3 m_offset{ 0.0f, 2.2f, 0.0f };  // 頭上の高さ
};

struct OtherEnemyHpBarBg : ObjectBase<OtherEnemyHpBarBgData>
{
	using ObjectBase<OtherEnemyHpBarBgData>::ObjectBase;

	void Start() override;
	void Update() override;

	void SetTargetPosition(const Vec3& _position);
};