#include "TransformAnimator.h"

TransformAnimator::TransformAnimator(
    std::initializer_list<std::pair<std::string, TransformAnimation>>&& _animations)
{
	for (auto& [key, animation] : _animations)
	{
		animations[key] = animation;
	}
}
