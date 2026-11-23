#pragma once
#include <string>

namespace cha
{
	// アニメーションのクリップ(Run, Walk など)
	struct AnimationClip
	{
		std::string name{};
		float duration{};
	};
}  // namespace cha