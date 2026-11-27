#pragma once
#include <Game/Game.h>
#include "GiftRequestSelectItemInfo.h"

/// @brief ギフトリクエスト選択要素のデータ
struct GiftRequestSelectElementData
{
	GiftRequestSelectElementData(
	    const Resource<Texture> _texture,
	    const float _beginAngleRad,
	    const float _sizeAngleRad,
	    const GiftRequestSelectItemInfo& _selectItemInfo) :
	    image{ _texture },
	    itemId{ _selectItemInfo.itemId },
	    count{ _selectItemInfo.count },
	    beginAngleRad{ _beginAngleRad },
	    sizeAngleRad{ _sizeAngleRad }
	{
		image.minRadius = 161.0f;
		image.maxRadius = 374.0f;
		image.beginAngleRad =
		    _beginAngleRad + DirectX::XM_PIDIV2;  ///< ImageCiecleComponentは0時開始だけど数学上は3時開始のためずらす
		image.sizeAngleRad = -_sizeAngleRad;
	}

	Component<UITransform> transform{ Vec2{ 587, 167 },
		                              Vec2{ 746, 746 },
		                              UILayerOrder{}.WithUser(10020) };  ///< UI座標系
	Component<UIImageCircle> image{};                                    ///< 画像コンポーネント
	ItemId itemId{};                                                     ///< アイテムId
	int32_t count{};                                                     ///< アイテムの個数
	GameObject objIcon{};                                                ///< アイテムのアイコンオブジェクト
	GameObject objText{};                                                ///< アイテムのテキスト
	float beginAngleRad{};                                               ///< 開始角度
	float sizeAngleRad{};                                                ///< 占める角度
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
