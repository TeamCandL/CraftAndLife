#pragma once
#include <Game/Game.h>
#include <nlohmann-json/json.hpp>

struct ImageObjectData
{
	ImageObjectData() = default;
	ImageObjectData(
	    const std::string_view _imageFilePath,
	    const Vec2& _begin,
	    const Vec2& _scale,
	    const int _layerOrder) :
	    texture{ _imageFilePath },
	    image{ texture },
	    transform{ _begin, _scale }
	{
		transform.layerOrder = _layerOrder;
	}
	ImageObjectData(
	    Resource<ResourceType::Texture> _texture,
	    const Vec2& _begin,
	    const Vec2& _scale,
	    const int _layerOrder) :
	    texture{ _texture },
	    image{ texture },
	    transform{ _begin, _scale }
	{
		transform.layerOrder = _layerOrder;
	}

	Resource<Texture> texture{};
	Component<UIImage> image{};
	Component<UITransform> transform{};
};
inline void from_json(
    const nlohmann::json& _j,
    ImageObjectData& _data)
{
	_j.at("x").get_to(_data.transform.position.x);
	_j.at("y").get_to(_data.transform.position.y);
	_j.at("width").get_to(_data.transform.scale.x);
	_j.at("height").get_to(_data.transform.scale.y);
}

struct ImageObject : ObjectBase<ImageObjectData>
{
	using ObjectBase<ImageObjectData>::ObjectBase;
};
