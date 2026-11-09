#pragma once
#include <Game/Game.h>
#include "ResultType.h"

struct ClearImageData
{
	Resource<Texture> texture;

	Component<UITransform> transform{};
	Component<UIImage> image{};

	ClearImageData(
	    ResultType _resultType)
	{
		if (_resultType == ResultType::Clear)
		{
			texture = Resource<Texture>{ "Image/Clear/Clear.png" };
		}
		else
		{
			texture = Resource<Texture>{ "Image/GAMEOVER/GAMEOVER.png" };
		}
	}
};

struct ClearImage : ObjectBase<ClearImageData>
{
	using ObjectBase<ClearImageData>::ObjectBase;

	void Start() override;
};