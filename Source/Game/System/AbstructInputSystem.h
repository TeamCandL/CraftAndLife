#pragma once
#include <Game/Game.h>

/// @brief 抽象的な入力を扱うシステムデータ
struct AbstructInputSystemData
{

};

/// @brief 抽象的な入力を扱うシステム
struct AbstructInputSystem : SystemObjectBase<AbstructInputSystem, AbstructInputSystemData>
{
	void Start() override;
	void Update() override;
};
