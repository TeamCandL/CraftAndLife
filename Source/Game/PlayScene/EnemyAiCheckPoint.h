#pragma once
#include <Game/Game.h>

struct EnemyCheckPointData
{
	EnemyCheckPointData() {}
	EnemyCheckPointData(
	    const Vec3 position_) :
	    transform{ position_ }
	{
	}

	Component<Transform> transform{};      ///< 座標系
	Component<RigidBody> rigidBody{};      ///< 剛体
	Component<StaticOBB> obb{};            ///< OBB当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	Vec3 OBB_OFFSET{ 0.0f, 0.1f, 0.0f };  ///< OBBのオフセット(位置)
	const float OBB_SCALE{ 0.25f };       ///< OBBのスケール
};

/// @brief 敵
struct EnemyCheckPoint : ObjectBase<EnemyCheckPointData>
{
	using ObjectBase<EnemyCheckPointData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	Vec3 GetPosition()
	{
		return transform.position;
	}
};