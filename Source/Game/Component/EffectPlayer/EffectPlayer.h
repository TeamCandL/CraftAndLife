#pragma once
#include <Game/System/EffectSystem.h>
/// @brief 座標系コンポーネント
struct EffectPlayer : ComponentBase<EffectPlayer>
{
	/// @brief デフォルトのコンストラクタ
	EffectPlayer() = default;
	// ループするか
	bool isLoop{ false };
	// 破棄するか
	bool destroyMe{ false };
	// エフェクトの再生速度
	float playSpeed{ 1.0f };
	void Reset();
	Effekseer::Handle handle{};
	Effekseer::EffectRef ref{ nullptr };
	double elapsedTime{ 0.0 };
};