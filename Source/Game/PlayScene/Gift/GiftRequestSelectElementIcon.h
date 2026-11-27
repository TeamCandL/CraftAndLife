#pragma once
#include <Game/game.h>

/// @brief ギフトリクエスト選択UIの各要素アイコンのデータ
struct GiftRequestSelectElementIconData
{

	GiftRequestSelectElementIconData(
	    const Resource<Texture>& _texture,
	    const Vec2 _centerPosition,
	    const float _scale = 64.0f) :
	    transform{ _centerPosition - Vec2{ 0.5f, 0.5f } * _scale,
		           Vec2::One() * _scale,
		           UILayerOrder{}.WithUser(10030) },
	    image{ _texture },
	    baseScale{ _scale },
	    focusedScale{ _scale * SELECTED_SCALE_RATIO },
	    centerPosition{ _centerPosition }
	{
	}

	inline static const float SELECTED_SCALE_RATIO{ 1.2f };  ///< 選択状態でのスケールレート

	Component<UITransform> transform{ Vec2{}, Vec2{}, UILayerOrder{}.WithUser(10030) };  ///< UI座標系
	Component<UIImage> image{};                                                          ///< UI画像
	float baseScale{};                                                                   ///< 基準となるアイコンのサイズ
	float focusedScale{};                                                                ///< 選択状態でのアイコンサイズ
	Vec2 centerPosition{};                                                               ///< 中心座標
};

/// @brief ギフトリクエスト選択UIの各要素アイコン
struct GiftRequestSelectElementIcon : ObjectBase<GiftRequestSelectElementIconData>
{
	using ObjectBase<GiftRequestSelectElementIconData>::ObjectBase;

	/// @brief 選択状態をセットする
	/// @param _isSelected 選択状態
	void SetFocus(const bool _isSelected);
};
