#pragma once
#include <Base/Base.h>

/// @brief Transformのアニメーション用のキー
struct TransformAnimationKey
{
	float timeAtSec{};      ///< キーの時間
	Vec3 position{};        ///< 座標
	Quaternion rotation{};  ///< 回転
	Vec3 scale{};           ///< 大きさ
};

/// @brief Transformのアニメーション
struct TransformAnimation
{
	std::vector<TransformAnimationKey> keys{};  ///< アニメーションキー
};

/// @brief Transformのアニメーションするやつ
struct TransformAnimator : ComponentBase<TransformAnimator>
{
	TransformAnimator() = default;

	std::unordered_map<std::string, std::vector<TransformAnimationKey>> animations{};  ///< アニメーションの登録
	std::string playAnimation{};                                                       ///< 再生するアニメーション名
	float playingTimeSec{};                                                            ///< 再生時間
	bool playToLoop{};  ///< 次もループ再生して！とお願いするフラグ
	bool isFinished{};  ///< 再生が終了しているか true / false
};
