#pragma once
#include <Base/Base.h>

/// @brief Transformのアニメーション用のキー
struct TransformAnimationKey
{
	float timeAtSec{};        ///< キーの時間
	Vec3 position{};          ///< 座標
	Quaternion quaternion{};  ///< 回転
	Vec3 scale{};             ///< 大きさ
};

/// @brief Transformのアニメーション
struct TransformAnimation
{
	std::vector<TransformAnimationKey> keys{};  ///< アニメーションキー
};

struct TransformAnimator : ComponentBase<TransformAnimator>
{
	TransformAnimator() = default;

	std::unordered_map<std::string, std::vector<TransformAnimationKey>> animations{};
};
