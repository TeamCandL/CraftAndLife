#pragma once
#include <Game/Game.h>

struct GitRequestSelectElementData
{
	GitRequestSelectElementData(
	    const Resource<Texture> _texture) :
	    image{ _texture }
	{
	}

	Component<UIImageCircle> image{};
};
struct GitRequestSelectElement : ObjectBase<GitRequestSelectElementData>
{
	void Start() override;
	void Update() override;
};
