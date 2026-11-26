#pragma once
#include <Game/Game.h>
#include "../ItemId.h"

/// @brief ギフトリクエスト選択要素のデータ
struct GiftRequestSelectElementData
{
	GiftRequestSelectElementData(
	    const Resource<Texture> _texture,
	    const float _beginAngleRad,
	    const float _endAngleRad,
	    const ItemId _itemId) :
	    image{ _texture },
	    itemId{ _itemId }
	{
		image.minRadius     = 161.0f;
		image.maxRadius     = 374.0f;
		image.beginAngleRad = _beginAngleRad;
		image.endAngleRad   = _endAngleRad;
	}

	Component<UITransform> transform{ Vec2{ 587, 167 },
		                              Vec2{ 746, 746 },
		                              UILayerOrder{}.WithUser(10020) };  ///< UI座標系
	Component<UIImageCircle> image{};                                    ///< 画像コンポーネント
	ItemId itemId{};                                                     ///< アイテムId
	GameObject objIcon{};                                                ///< アイテムのアイコンオブジェクト
};

/// @brief ギフトリクエスト選択要素
struct GiftRequestSelectElement : ObjectBase<GiftRequestSelectElementData>
{
	using ObjectBase<GiftRequestSelectElementData>::ObjectBase;

	void Start() override;
	void Update() override;
	void OnDestroy() override;

	/// @brief フォーカスされているかをセットする
	/// @param _focused フォーカスされている true / false
	void SetFocus(const bool _focused);
};
