#pragma once
#include <Base/Base.h>

struct UITransform;
struct Transform;

/// @brief UITransformとTransformを結びつけワールド座標に対応したスクリーン座標に描画する
struct UIWorldBinder : ComponentBase<UIWorldBinder>
{
	inline UIWorldBinder(
	    UITransform& _uiTransform,
	    Transform& _transform) :
	    uiTransform{ _uiTransform },
	    transform{ _transform }
	{
	}

	UITransform& uiTransform;
	Transform& transform;

	Vec2 baseScale{};     ///< 元のスケール
	Vec2 basePosition{};  ///< 元のUI座標
};
