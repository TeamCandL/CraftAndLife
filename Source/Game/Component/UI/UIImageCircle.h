#pragma once
#include <Base/Base.h>

/// @brief UI画像の円描画コンポーネント
struct UIImageCircle : ComponentBase<UIImageCircle>
{
	UIImageCircle() {}

	explicit UIImageCircle(
	    const Resource<ResourceType::Texture>& _texture) :
	    texture{ _texture },
	    baseColor{ Color::WHITE },
	    addColor{ Color::CLEAR }
	{
	}
	~UIImageCircle() = default;

	Resource<ResourceType::Texture> texture{};  ///< テクスチャ
	Color baseColor{};                          ///< ベースの色
	Color addColor{};                           ///< ベースの加法混色
	float minRadius{};                          ///< 円中心からの切り抜き半径
	float maxRadius{};                          ///< 円中心からの表示半径
	float beginAngleRad{};                      ///< 円の開始地点
	float endAngleRad{};                        ///< 円の終了地点
};
