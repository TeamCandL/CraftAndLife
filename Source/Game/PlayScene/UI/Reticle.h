#pragma once
#include <Game/Game.h>

/// @brief レティクルのデータ
struct ReticleData
{
	Resource<Texture> texture{ "Image/UI/reticle.png" };  ///< テクスチャ

	Component<UITransform> transform{};  ///< UI座標系
	Component<UIImage> image{};          ///< UI画像
};

/// @brief レティクル
struct Reticle : ObjectBase<ReticleData>
{
	/// @brief 開始処理
	void Start() override;
};
