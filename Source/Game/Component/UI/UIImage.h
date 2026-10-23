#pragma once
#include <Base/Base.h>

/// @brief UI画像の描画コンポーネント
struct UIImage : ComponentBase<UIImage>
{
	UIImage() {}

	UIImage(
	    const Resource<ResourceType::Texture>& _texture) :
	    texture{ _texture }
	{
	}

	Resource<ResourceType::Texture> texture{};  // テクスチャ
	Color baseColor{};                          // ベースの色
};
