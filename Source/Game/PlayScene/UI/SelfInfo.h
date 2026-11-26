
#pragma once
#include <Game/Game.h>

struct SelfInfoData
{
	SelfInfoData(
	    GameObject _objPlayer) :
	    objPlayer{ _objPlayer }
	{
	}

	GameObject objPlayer{};

	// 現在表示している役職
	int32_t currentRole{ -1 };

	Resource<Texture> texture{};

	Component<UITransform> transform{ Vec2{ 136, 917 }, Vec2{ 573, 120 } };
	Component<UIImage> image{ texture };
};

struct SelfInfo : ObjectBase<SelfInfoData>
{
	using ObjectBase<SelfInfoData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
};
