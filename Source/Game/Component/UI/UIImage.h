#pragma once
#include <Base/Base.h>

/// @brief UI画像の描画コンポーネント
struct UIImage : ComponentBase<UIImage>
{
	UIImage() {}

	explicit UIImage(
	    const Resource<ResourceType::Texture>& _texture) :
	    texture{ _texture }
	{
	}
	~UIImage() = default;

	Resource<ResourceType::Texture> texture{};  ///< テクスチャ
	Color baseColor{ Color::WHITE };            ///< ベースの色
	Color addColor{ Color::CLEAR };             ///< 加法混色
};
