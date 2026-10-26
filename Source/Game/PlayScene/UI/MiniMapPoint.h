#pragma once
#include <Game/Game.h>

/// @brief ミニマップ上のポイントマークのデータ
struct MiniMapPointData
{
	MiniMapPointData(
	    const Vec2 _imageSize,
	    const fs::path& _imagePath) :
	    transform{ Vec2::Zero(), _imageSize },
	    image{ _imagePath }
	{
	}

	Component<UITransform> transform{};  ///< UI座標系
	Component<UIImage> image{};          ///< UI画像
	                                     // Vec2
};

/// @brief ミニマップ上のポイントマーク
struct MiniMapPoint : ObjectBase<MiniMapPointData>
{
	/// @brief 極座標系でミニマップ上の座標を指定する
	/// @param _angle 角度
	/// @param _radius 半径(ミニマップから画像の中心まで)
	void SetPosition(const float _angle, const float _radius);
};
