#pragma once
#include <Game/game.h>

/// @brief ギフトリクエスト選択UIの各要素アイコンのデータ
struct GiftRequestSelectElementTextData
{
	Resource<Font> font{ FontLoadConfig{}
		                     .WithFile("Font/Stick/Stick-Regular.ttf")
		                     .WithFamilyName(L"Stick")
		                     .WithFontSize(24)
		                     .WithFontAlignment(FontAlignment_BottomRight) };

	GiftRequestSelectElementTextData(
	    const std::string& _content,
	    const Vec2 _centerPosition,
	    const float _scale = 74.0f) :
	    transform{ _centerPosition - Vec2{ 0.5f, 0.5f } * _scale,
		           Vec2::One() * _scale,
		           UILayerOrder{}.WithUser(10030) },
	    text{ font, Color::WHITE },
	    baseScale{ Vec2::One() * _scale },
	    focusedScale{ Vec2::One() * _scale * SELECTED_SCALE_RATIO }
	{
		text.content = _content;
	}

	inline static const float SELECTED_SCALE_RATIO{ 1.2f };  ///< 選択状態でのスケールレート

	Component<UITransform> transform{ Vec2{}, Vec2{}, UILayerOrder{}.WithUser(10030) };  ///< UI座標系
	Component<UIText> text{};                                                            ///< UI画像
	Vec2 baseScale{};                                                                    ///< 基準となるアイコンのサイズ
	Vec2 focusedScale{};                                                                 ///< 選択状態でのアイコンサイズ
};

/// @brief ギフトリクエスト選択UIの各要素アイコン
struct GiftRequestSelectElementText : ObjectBase<GiftRequestSelectElementTextData>
{
	using ObjectBase<GiftRequestSelectElementTextData>::ObjectBase;

	/// @brief 選択状態をセットする
	/// @param _isSelected 選択状態
	// void SetSelected(const bool _isSelected);
};
