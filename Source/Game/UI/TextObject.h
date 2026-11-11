#pragma once
#include <Game/Game.h>
#include <nlohmann-json/json.hpp>
struct TextObjectData
{
	TextObjectData() = default;
	TextObjectData(
	    const Vec2& _begin,
	    const Vec2& _scale,
	    const int _layerOrder,
	    Resource<ResourceType::Font> _font = FontLoadConfig{}) :
	    transform{ _begin, _scale }
	{
		transform.layerOrder = _layerOrder;
		text.font            = _font;
	}
	Component<UITransform> transform{};
	Component<UIText> text{};
};
inline void from_json(
    const nlohmann::json& _j,
    TextObjectData& _data)
{
	_j.at("x").get_to(_data.transform.position.x);
	_j.at("y").get_to(_data.transform.position.y);
	_j.at("width").get_to(_data.transform.scale.x);
	_j.at("height").get_to(_data.transform.scale.y);
	_data.text.content = _j.at("content").get<std::string>();

	FontLoadConfig config;
	config.fontSize = _j.at("fontSize").get<float>();

	Resource<ResourceType::Font> font{ config };
	_data.text.font = font;
}
struct TextObject : ObjectBase<TextObjectData>
{
	using ObjectBase<TextObjectData>::ObjectBase;
	void SetContent(const std::string& _content);
};
