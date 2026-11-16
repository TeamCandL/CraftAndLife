#pragma once
#include <Game/Game.h>

/// @brief UIをワールド空間と紐づけるシステム
struct UIWorldBindSystem : SystemObjectBase<UIWorldBindSystem>
{
	/// @brief 遅れた更新処理
	void LateUpdate() override;
};
