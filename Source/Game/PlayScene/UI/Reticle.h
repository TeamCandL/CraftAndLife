#pragma once
#include <Game/Game.h>

/// @brief レティクルのデータ
struct ReticleData
{
	Resource<Texture> texture{ "UI/Image/Reticle_CrossHair.png" };  ///< テクスチャ

	Component<UITransform> transform{};  ///< UI座標系
	Component<UIImage> image{};          ///< UI画像
};

/// @brief レティクル
struct Reticle : ObjectBase<ReticleData>
{
	/// @brief 開始処理
	void Start() override;
};