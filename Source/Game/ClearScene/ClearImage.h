#pragma once
#include <Game/Game.h>

/// @brief クリア画像のデータ
struct ClearImageData
{
	Resource<Texture> texture{ "Image/Clear/Clear.png" };

	Component<UITransform> transform{};
	Component<UIImage> image{};
};

/// @brief クリア画像
struct ClearImage : ObjectBase<ClearImageData>
{
	void Start() override;
};