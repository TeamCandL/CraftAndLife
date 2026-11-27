#pragma once
#include <Game/Game.h>

/// @brief ギフトリクエスト選択中の中央テキストのデータ
struct GiftRequestSelectCenterBackgroundData
{
	Component<UITransform> transform{ Vec2{ 587 + 207, 167 + 209 }, Vec2{ 331, 327 } };     ///< UI座標系
	Component<UIImage> image{ "Image/UI/Gift/gift_request_select_center_background.png" };  ///< UI画像
};

/// @brief ギフトリクエスト選択中の中央テキスト
struct GiftRequestSelectCenterBackground : ObjectBase<GiftRequestSelectCenterBackgroundData>
{
};
