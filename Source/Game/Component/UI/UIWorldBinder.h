#pragma once
#include <Base/Base.h>

struct UITransform;
struct Transform;

/// @brief UITransformとTransformを結びつけワールド座標に対応したスクリーン座標に描画する
struct UIWorldBinder : ComponentBase<UIWorldBinder>
{
	inline UIWorldBinder(
	    UITransform& _uiTransform,
	    Transform& _transform,
	    const float _fixedScaleDistance = 0.0f) :
	    uiTransform{ _uiTransform },
	    transform{ _transform },
	    fixedScaleDistance{ _fixedScaleDistance }
	{
	}

	UITransform& uiTransform;
	Transform& transform;

	float fixedScaleDistance{};

	Vec2 baseScale{};     ///< 元のスケール
	Vec2 basePosition{};  ///< 元のUI座標
};
