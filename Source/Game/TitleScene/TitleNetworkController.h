#pragma once
#include <Game/Game.h>

struct TitleNetworkControllerData
{
};

struct TitleNetworkController : ObjectBase<TitleNetworkControllerData>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
};
