#pragma once
#include <Game/Game.h>

struct GitRequestSelectElementData
{
	// Component<UIImageCircle>
};
struct GitRequestSelectElement : ObjectBase<GitRequestSelectElementData>
{
	void Start() override;
	void Update() override;
};
