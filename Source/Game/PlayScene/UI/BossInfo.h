#pragma once
#include <Game/Game.h>

struct HpInfo
{
	int32_t maxHP{};   ///< ボスの最大HP
	int32_t hp{};      ///< ボスの現在HP
	float rateOfHp{};  ///< ボスのHPの割合
};

struct BossInfoData
{
	Resource<Texture> texture{ "Image/UI/boss_info.png" };

	Component<UITransform> transform{ Vec2{ 657, 0 }, Vec2{ 577, 100 } };
	Component<UIImage> image{ texture };

	GameObject objBoss{};        ///< ボスのオブジェクト
	int32_t bossEnemyId{ -1 };   ///< ボスの敵ID
	HpInfo hpInfo{};             ///< ボスのHP情報
	bool isBossActive{ false };  ///< ボスがアクティブかどうか
};

struct BossInfo : ObjectBase<BossInfoData>
{
	// using ObjectBase<BossInfoData>::ObjectBase;
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;

	// ホスト用
	void SetBoss(GameObject objBoss_);

	// ゲスト用
	void SetBoss(int32_t enemyId, int32_t maxHP);

	bool IsBossActive() const
	{
		return isBossActive;
	}

	HpInfo GetHpInfo() const
	{
		return hpInfo;
	}
};
