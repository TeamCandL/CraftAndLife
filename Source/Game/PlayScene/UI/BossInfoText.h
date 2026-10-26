#pragma once
#include <Game/Game.h>

struct BossInfoTextData
{
	Component<UITransform> transform{};
	Component<UIText> text{};
};
struct BossInfoText : ObjectBase<BossInfoTextData>
{
	// void Strat() override;
};
